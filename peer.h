#pragma once

#include "inode.h"
#include "message.pb.h"
#include <boost/asio.hpp>
#include <optional>

using udp = boost::asio::ip::udp;

static constexpr std::size_t MAX_DGRAM = 1200;  // safe under typical MTU

// struct Endpoint {
//     std::string host;
//     uint32_t    port;
// };

// static inline std::optional<Endpoint> parse_host_port(
//     const std::string& host_port) {
//     auto pos = host_port.find(':');
//     if (pos == std::string::npos)
//         return std::nullopt;
//     Endpoint e{host_port.substr(0, pos),
//                static_cast<uint16_t>(std::stoi(host_port.substr(pos + 1)))};
//     return e;
// }

class Peer {
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

   public:
    Peer(boost::asio::io_context& io,
         std::vector<PeerInfo>    boot_nodes,
         std::string              host,
         uint32_t                 port = 0);

    const PeerInfo& get_peer_info() { return info_; }

    void receive_loop();

    void bootstrap();

    void on_datagram(const uint8_t*       data,
                     std::size_t          bytes_received,
                     const udp::endpoint& from);

    void send(const Message& m);

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