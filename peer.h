#pragma once

#include "ipeer.h"

static constexpr std::size_t MAX_DGRAM = 1200;  // safe under typical MTU

class Peer : public IPeer {
   private:
    boost::asio::io_context&       io_;
    udp::socket                    socket_;
    std::vector<PeerInfo>          boot_nodes_;
    std::array<uint8_t, MAX_DGRAM> rx_buf_;
    // boost::asio::steady_timer ping_timer_;

    std::unique_ptr<INode> node_ = nullptr;
    PeerInfo               info_;
    uint64_t               nonce_ = 0;
    udp::endpoint          rx_from_;
    std::string            name_;

    std::unordered_map<uint64_t, std::shared_ptr<LookupContext>> lookups_;

   public:
    Peer(boost::asio::io_context& io,
         std::vector<PeerInfo>    boot_nodes,
         std::string              host,
         uint32_t                 port = 0);

    const PeerInfo& getPeerInfo() override { return info_; }

    void receiveLoop() override;

    void bootstrap() override;

    void onDatagram(const uint8_t*       data,
                    std::size_t          bytes_received,
                    const udp::endpoint& from) override;

    void send(const Message& m) override;
    void endLookup(uint64_t nonce) override;

    void handleFindNodeQuery(const Message& msg) override;
    void handleFindNodeReply(const Message& msg) override;
//     std::shared_ptr<LookupContext> createLookupContext(const PeerInfo& sender,
//                                                        NodeId          target,
//                                                        uint64_t nonce) override;

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