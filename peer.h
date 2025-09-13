#pragma once

#include "inode.h"
#include "ipeer.h"
#include "lookupStats.h"
#include "lookupcontext.h"

class Peer : public IPeer {
   private:
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

   public:
    Peer(boost::asio::io_context&     io,
         std::string                  host,
         std::shared_ptr<LookupStats> stats,
         uint32_t                     port   = 0,
         bool                         isBoot = false);

    Peer(boost::asio::io_context&     io,
         std::string                  host,
         std::shared_ptr<LookupStats> stats,
         uint64_t                     id1,
         uint64_t                     id2,
         uint32_t                     port   = 0,
         bool                         isBoot = false);

    const PeerInfo& getPeerInfo() override { return info_; }

    void receiveLoop() override;

    void bootstrap() override;
    void find(const NodeId& id) override;
    void insert(const PeerInfo& pi) override;

    void start() override;

    void onDatagram(const uint8_t*       data,
                    std::size_t          bytes_received,
                    const udp::endpoint& from) override;

    void send(const Message& m) override;
    void startContext(uint64_t                       nonce,
                      std::shared_ptr<LookupContext> ctx) override;
    void endContext(uint64_t nonce) override;

    const std::array<uint8_t, MAX_DGRAM>& getBuffer() override {
        return rx_buf_;
    }
    const udp::endpoint& getSender() override { return rx_from_; }
    const boost::asio::strand<boost::asio::io_context::executor_type>&
    getStrand() override {
        return strand_;
    }
    const std::unique_ptr<INode>&  getNode() const override { return node_; }
    std::shared_ptr<LookupContext> getContext(uint64_t nonce) override;
    boost::asio::io_context&       getIo() override { return io_; }
    std::shared_ptr<LookupStats>   getStats() override { return stats_; }

    //     std::shared_ptr<LookupContext> createLookupContext(const PeerInfo&
    //     sender,
    //                                                        NodeId target,
    //                                                        uint64_t nonce)
    //                                                        override;

    // void schedule_ping_seeds() {
    //     ping_timer_.expires_after(std::chrono::seconds(2));
    //     ping_timer_.async_wait([this](boost::system::error_code ec) {
    //         if (ec)
    //             return;
    //         for (auto& s : boot_nodes) {
    //             if (auto endpoint = parse_host_port(s))
    //                 send_ping(*endpoint, "hi");
    //         }
    //         schedule_ping_seeds();
    //     });
    // }
};