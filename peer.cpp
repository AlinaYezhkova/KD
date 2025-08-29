#include "peer.h"

#include "constants.h"
#include "messageBuilder.h"
#include "node.h"
#include "utils.h"

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
    port     = socket_.local_endpoint().port();

    name_ = host + ":" + std::to_string(port);
    // fmt::println("Peer running at {}...", name_);
    node_ = std::make_unique<Node>(name_);
    info_ = {node_->get_id(), endpoint, get_current_timestamp()};
    bootstrap();
    receiveLoop();
}

void Peer::receiveLoop() {
    socket_.async_receive_from(
        boost::asio::buffer(rx_buf_),
        rx_from_,
        [this](boost::system::error_code ec, std::size_t bytes_received) {
            if (!ec) {
                onDatagram(rx_buf_.data(), bytes_received, rx_from_);
            }
            receiveLoop();
        });
}

void Peer::onDatagram(const uint8_t*       data,
                      std::size_t          bytes_received,
                      const udp::endpoint& from) {
    Message msg;
    if (!msg.ParseFromArray(data, static_cast<int>(bytes_received))) {
        std::cerr << "parse error from " << from << " size =" << bytes_received
                  << "\n";
        return;
    }

    node_->insert(peerInfoFromProto(msg.from_user()));

    // fmt::println("{} received \"{}\" from {} ( nonce = {} )",
                //  info_.key_,
                //  magic_enum::enum_name(msg.type()),
                //  msg.from_user().key(),
                //  msg.nonce());

    switch (msg.type()) {
        case Find_node_query:
            handleFindNodeQuery(msg);
            break;

        case Find_node_reply:
            handleFindNodeReply(msg);
            break;

        default:
            std::cerr << "Unknown msg type\n";
    }
}

void Peer::handleFindNodeQuery(const Message& msg) {
    PeerInfo sender = peerInfoFromProto(msg.from_user());
    NodeId   target = nodeIdFromProto(msg.find_user());
    uint64_t nonce  = msg.nonce();

    std::vector<PeerInfo> closest = node_->find_closest(target);

    Message response = MessageBuilder()
                           .type(MessageType::Find_node_reply)
                           .from(info_)
                           .to(sender)
                           .result(closest)
                           .nonce(nonce)
                           .build();

    send(response);
}

void Peer::handleFindNodeReply(const Message& msg) {
    // fmt::println("nonce = {}", msg.nonce());
    auto ctx = lookups_.find(msg.nonce());
    if (ctx == lookups_.end()) {
        // throw std::runtime_error("wrong nonce or context unavailable");
        return;
    }
    std::vector<PeerInfo> result = resultFromProto(msg);
    ctx->second->onResponse(result);
}

void Peer::bootstrap() {
    if (!boot_nodes_.empty()) {
        // fmt::println("{} ({}) bootstrapping", name_, info_.key_);
    }
    for (const auto& pi : boot_nodes_) {
        node_->insert(pi);

        uint64_t nonce = random_nonce();
        auto     ctx =
            std::make_shared<LookupContext>(info_.key_, *this, *node_, nonce);
        ctx->increment_flight();
        lookups_[nonce] = ctx;
        // fmt::println("!!! initiated '{}' context", nonce);

        Message msg = MessageBuilder()
                          .type(MessageType::Find_node_query)
                          .from(info_)
                          .to(pi)
                          .find(info_.key_)
                          .nonce(nonce)
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

void Peer::endLookup(uint64_t nonce) {
    lookups_.erase(nonce);
}