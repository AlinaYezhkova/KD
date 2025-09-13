#include "peer.h"

#include "constants.h"
#include "messageBuilder.h"
#include "messageHandler.h"
#include "node.h"
#include "swarm.h"
#include "utils.h"

/*
    boost::asio::io_context&                                    io_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    udp::socket                                                 socket_;
    std::array<uint8_t, MAX_DGRAM>                              rx_buf_;
    std::shared_ptr<LookupStats> stats_ = nullptr;

    boost::asio::steady_timer ping_timer_;
    bool                      isBoot_ = false;
    std::unique_ptr<INode>    node_   = nullptr;
    PeerInfo                  info_;
    uint64_t                  nonce_ = 0;
    udp::endpoint             rx_from_;
    std::string               name_;

    std::unordered_map<uint64_t, std::shared_ptr<LookupContext>> lookups_;
*/

Peer::Peer(boost::asio::io_context&     io,
           std::string                  host,
           std::shared_ptr<LookupStats> stats,
           uint32_t                     port,
           bool                         isBoot)
  : io_(io)
  , strand_(boost::asio::make_strand(io))
  , socket_(strand_)
  , rx_buf_{}
  , stats_(std::move(stats))
  , ping_timer_(strand_)
  , isBoot_(isBoot) {
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
}

Peer::Peer(boost::asio::io_context&     io,
           std::string                  host,
           std::shared_ptr<LookupStats> stats,
           uint64_t                     id1,
           uint64_t                     id2,
           uint32_t                     port,
           bool                         isBoot)
  : io_(io)
  , strand_(boost::asio::make_strand(io))
  , socket_(strand_)
  , rx_buf_{}
  , stats_(std::move(stats))
  , ping_timer_(strand_)
  , isBoot_(isBoot) {
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
    node_ = std::make_unique<Node>(id1, id2);
    info_ = {node_->get_id(), endpoint, get_current_timestamp()};
}

void Peer::start() {
    if (!isBoot_) {
        bootstrap();
    }
    receiveLoop();
}

void Peer::receiveLoop() {
    socket_.async_receive_from(
        boost::asio::buffer(rx_buf_),
        rx_from_,
        boost::asio::bind_executor(
            strand_,
            [self = shared_from_this()](boost::system::error_code ec,
                                        std::size_t               n) {
                if (!ec)
                    self->onDatagram(self->getBuffer().data(),
                                     n,
                                     self->getSender());
                self->receiveLoop();
            }));
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
    MessageHandler mh;
    mh.handle(*this, msg);
}

void Peer::bootstrap() {
    auto& swarm = Swarm::getInstance();
    auto  self  = shared_from_this();

    // This handler runs on Swarm's strand (NOT on this peer's strand)

    // swarm.async_getClosestPeer(info_.key_, [self](std::shared_ptr<IPeer> p) {
    swarm.async_getRandomPeer([self](std::shared_ptr<IPeer> p) {
        // Hop back to *this peer's* strand before touching its state
        boost::asio::dispatch(self->getStrand(),
                              [self, p = std::move(p)]() mutable {
                                  // SAFE: we're now on this peer's strand
                                  PeerInfo pi = p->getPeerInfo();
                                  self->getNode()->insert(pi);

                                  Message query =
                                      MessageBuilder()
                                          .type(MessageType::Bootstrap_query)
                                          .from(self->getPeerInfo())
                                          .to(pi)
                                          .build();

                                  self->send(query);
                              });
    });
}

void Peer::find(const NodeId& id) {
    boost::asio::dispatch(strand_, [self = shared_from_this(), id] {
        if (id == self->getPeerInfo().key_) {
            // looking for oneself
            return;
        }
        uint64_t nonce = random_nonce();
        auto     ctx   = std::make_shared<LookupContext>(id,
                                                   *self,
                                                   *self->getNode(),
                                                   nonce,
                                                   self->getStats());
        self->startContext(nonce, ctx);
    });
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
    try {
        auto to = endpointFromProto(msg.to_user());
        socket_.async_send_to(
            boost::asio::buffer(*buf),
            to,
            boost::asio::bind_executor(
                strand_,
                [buf](boost::system::error_code ec, std::size_t) {
                    if (ec)
                        std::cerr << "[tx] send error: " << ec.message()
                                  << "\n";
                }));

    } catch (const std::exception& e) {
        std::cerr << "[tx] exception in send: " << e.what() << "\n";
    }
}

void Peer::endContext(uint64_t nonce) {
    lookups_.erase(nonce);
}

void Peer::startContext(uint64_t nonce, std::shared_ptr<LookupContext> ctx) {
    lookups_.emplace(nonce, ctx);
    ctx->start();
}

std::shared_ptr<LookupContext> Peer::getContext(uint64_t nonce) {
    auto ctx_ptr = lookups_.find(nonce);
    if (ctx_ptr == lookups_.end()) {
        return nullptr;
    }
    return ctx_ptr->second;
}

// void Peer::scheduleBootstrapRetry(std::chrono::milliseconds d) {
//     retry_timer_.expires_after(d);
//     retry_timer_.async_wait(
//         boost::asio::bind_executor(
//             strand_,
//             [self = shared_from_this()](const boost::system::error_code& ec)
//             {
//                 if (!ec) self->bootstrap();
//             }));
// }

void Peer::insert(const PeerInfo& pi) {
    node_->insert(pi);
}
