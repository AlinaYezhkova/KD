#include "peer.h"

#include "constants.h"
#include "lookupcontext.h"
#include "messageBuilder.h"
#include "node.h"

Peer::Peer(boost::asio::io_context& io,
           std::vector<PeerInfo>    boot_nodes,
           std::string              host,
           uint32_t                 port)
  : io_(io)
  , socket_(io)
  , boot_nodes_(std::move(boot_nodes))
  , rx_buf_{}  //   , ping_timer_(io)
{
    boost::system::error_code ec;

    auto addr = boost::asio::ip::make_address(host, ec);
    if (ec) {
        throw std::runtime_error("bad host: " + host + " (" + ec.message() +
                                 ")");
    }

    udp::endpoint endpoint(addr, port);
    socket_.open(endpoint.protocol(), ec);
    if (ec) {
        throw std::runtime_error("open: " + ec.message());
    }

    socket_.bind(endpoint, ec);
    if (ec) {
        throw std::runtime_error("bind: " + ec.message());
    }

    endpoint = socket_.local_endpoint();
    port = socket_.local_endpoint().port();

    std::string addr_str = host + ":" + std::to_string(port);
    fmt::println("Peer running at {}...", addr_str);
    node_ = std::make_unique<Node>(addr_str);
    info_ = {node_->get_id(), endpoint, get_current_timestamp()};
    if (!boot_nodes_.empty()) {
        bootstrap();
    }
    receive_loop();
}

void Peer::receive_loop() {
    socket_.async_receive_from(
        boost::asio::buffer(rx_buf_),
        rx_from_,
        [this](boost::system::error_code ec, std::size_t bytes_received) {
            if (!ec) {
                on_datagram(rx_buf_.data(), bytes_received, rx_from_);
            }
            receive_loop();
        });
}

void Peer::on_datagram(const uint8_t*       data,
                       std::size_t          bytes_received,
                       const udp::endpoint& from) {
    Message msg;
    if (!msg.ParseFromArray(data, static_cast<int>(bytes_received))) {
        std::cerr << "parse error from " << from << " size =" << bytes_received
                  << "\n";
        return;
    }

    MessageType type = msg.type();

    // if (msg.type() == MessageType::Find_node_answer) {
    //     auto                  from   = peerInfoFromProto(msg.from_user());
    //     std::vector<PeerInfo> result = extract_result_from_msg(msg);
    //     std::vector<PeerInfo> closer =
    //         extract_closer_nodes_from_msg(msg);  // optional

    //     // Now call:
    //     ctx->onResponse(from, result, closer);
    // }

    PeerInfo sender   = peerInfoFromProto(msg.from_user());
    PeerInfo receiver = peerInfoFromProto(msg.to_user());

    NodeId target = nodeIdFromProto(msg.find_user());

    /*
      , find_closest_(std::move(find_closest))
      , send_query_(std::move(send_query))
      , on_done_(std::move(on_done))
      , should_stop_(std::move(should_stop))
      , comp_([target](const NodeId& a, const NodeId& b) {
          return distance(a, target) < distance(b, target);
      })

    */
    auto ctx = std::make_shared<LookupContext>(
        target,

        // FindClosestFn
        [this](NodeId target) -> std::vector<PeerInfo> {
            return node_->find_closest(target);
        },

        // SendQueryFn
        [this, sender, target](const PeerInfo& pi) {
            Message out = MessageBuilder()
                              .type(MessageType::Find_node_query)
                              .from(sender)  // current node ID
                              .to(pi)        // where it's going
                              .find(target)  // who we're looking for
                              .build();
            send(out);
        },

        // OnResultFn
        [this](const std::map<NodeId, PeerInfo, Comparator>& peers) {
            for (const auto& [id, pi] : peers) node_->insert(pi);
        },

        // ShouldStopFn
        [](const std::map<NodeId, PeerInfo, Comparator>& current_best) -> bool {
            // Stop when you found at least α or k nodes, or if result doesn't
            // improve
            return current_best.size() >= kReturn;  // or use convergence logic
        });

    ctx->start();
}

void Peer::bootstrap() {
    for (const auto& pi : boot_nodes_) {
        // store the boot node in myself
        node_->insert(pi);

        // send pi FIND_NODE(myself)
        Message msg = MessageBuilder()
                          .type(MessageType::Find_node_query)
                          .from(info_)
                          .to(pi)
                          .find(info_.key_)
                          .build();

        send(msg);
    }
}

void Peer::send(const Message& msg) {
    int sz = msg.ByteSizeLong();
    if (sz <= 0 || sz > MAX_DGRAM) {
        std::cerr << "[tx] message too big: " << sz << " bytes\n";
        return;
    }

    auto buf = std::make_shared<std::vector<uint8_t>>(sz);
    if (!msg.SerializeToArray(buf->data(), sz)) {
        std::cerr << "[tx] serialize failed\n";
        return;
    }

    // Convert PeerInfo to endpoint
    try {
        auto to = endpointFromProto(msg.to_user());
        socket_.async_send_to(
            boost::asio::buffer(*buf),
            to,
            [buf](boost::system::error_code ec, std::size_t) {
                if (ec)
                    std::cerr << "[tx] send error: " << ec.message() << "\n";
            });
    } catch (const std::exception& e) {
        std::cerr << "[tx] exception in send: " << e.what() << "\n";
    }
}
