#pragma once

#include <fmt/core.h>

#include "constants.h"
#include "inode.h"
#include "message.pb.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <string_view>

static const char* g_file_path = "/home/odal/QtProjects/KD/log.txt";

inline std::mutex g_log_mutex;

static int64_t get_current_timestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

inline void log_impl(fmt::string_view fmt_str, fmt::format_args args) {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    static std::ofstream        out(g_file_path, std::ios::app);
    if (!out) {
        std::cerr << "No log file.\n";
        return;
    }
    out << fmt::vformat(fmt_str, args) << '\n';
    out.flush();  // make visible immediately
}

template <typename... Args>
inline void log_fmt(fmt::format_string<Args...> fmt_str, Args&&... args) {
    log_impl(fmt_str, fmt::make_format_args(args...));
}

#define LOG(...) log_fmt(__VA_ARGS__)

std::string toBinaryString(uint64_t value);

inline std::array<uint64_t, 2> xor_id(const NodeId& a, const NodeId& b) {
    return {a[0] ^ b[0], a[1] ^ b[1]};
}

inline int distance(const NodeId a, const NodeId b) {
    uint64_t x0 = a[0] ^ b[0];  // high 64 bits
    if (x0 != 0) {
        return 127 - std::countl_zero(x0);  // MSB found in upper half
    }
    uint64_t x1 = a[1] ^ b[1];  // low 64 bits
    if (x1 != 0) {
        return 63 - std::countl_zero(x1);  // MSB found in lower half
    }
    return 0;  // identical IDs (may be -1)
}

//----------------------- toProto -----------------------
// inline PeerInfoProto toProto(const std::string&   key_bytes,
//                              const udp::endpoint& ep) {
//     if (!ep.address().is_v4()) {
//         throw std::runtime_error("PeerInfo expects IPv4 address");
//     }
//     PeerInfoProto m;

//     // key: что угодно бинарное
//     m.set_key(key_bytes.data(), static_cast<int>(key_bytes.size()));

//     // addr: ровно 4 байта IPv4
//     auto a4 = ep.address().to_v4().to_bytes();  // std::array<uint8_t,4>
//     m.set_addr(a4.data(), a4.size());

//     // port: 0..65535 хранится как uint32
//     m.set_port(ep.port());
//     return m;
// }

// // Перегрузка, если ключ у вас как массив/вектор байт:
// template <class ByteContainer>
// inline PeerInfoProto toProto(const ByteContainer& key,
//                              const udp::endpoint& ep) {
//     PeerInfoProto m =
//         toProto(std::string(reinterpret_cast<const char*>(key.data()),
//                             static_cast<int>(key.size())),
//                 ep);
//     return m;
// }

// inline PeerInfoProto toProto(const PeerInfo& pi) {
//     PeerInfoProto m = toProto(pi.key_, pi.endpoint_);
//     return m;
// }

//----------------------- fromProto -----------------------
inline boost::asio::ip::udp::endpoint endpointFromProto(
    const PeerInfoProto& m) {
    if (m.addr().size() != IPv4Size) {
        throw std::invalid_argument("PeerInfo.addr must be 4 bytes for IPv4");
    }

    boost::asio::ip::address_v4::bytes_type bytes;
    std::copy_n(m.addr().begin(), IPv4Size, bytes.begin());

    return {boost::asio::ip::address_v4(bytes),
            static_cast<unsigned short>(m.port())};
}

inline std::array<uint64_t, 2> nodeIdFromProto(const PeerInfoProto& m) {
    return {m.key().high(), m.key().low()};
}
inline std::array<uint64_t, 2> nodeIdFromProto(const NodeIdProto& m) {
    return {m.high(), m.low()};
}

inline PeerInfo peerInfoFromProto(const PeerInfoProto& m) {
    return {nodeIdFromProto(m), endpointFromProto(m), get_current_timestamp()};
}

// Если нужно забрать key в «сырых» байтах:
// inline std::vector<uint8_t> keyBytesFromProto(const PeerInfoProto& m) {
//     const auto& s = m.key();  // std::string с бинарными данными
//     return std::vector<uint8_t>(s.begin(), s.end());
// }