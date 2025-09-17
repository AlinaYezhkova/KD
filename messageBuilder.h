#pragma once

#include <fmt/chrono.h>
#include <fmt/core.h>

#include "message.pb.h"
#include <magic_enum/magic_enum.hpp>
#include "utils.h"
/*
message PeerInfoProto {
    bytes key = 1;
    bytes addr = 2;    // must be 4 bytes
    uint32 port = 3;

    bytes name = 4;
}
*/

// std::array<uint8_t, size> to_bytes(const std::bitset<kIdLength>& b) {
//     std::array<uint8_t, size> out{};
//     out.fill(0);
//     for (std::size_t i = 0; i < kIdLength; ++i) {
//         if (b.test(i))
//             out[i / 8] |= static_cast<uint8_t>(1u << (i % 8));  // LSB-first
//     }
//     return out;
// }

class PeerInfoBuilder {
   private:
    PeerInfoProto pi_;

   public:
    // PeerInfoBuilder& key(const std::bitset<kIdLength>& bits) {
    //     auto bytes = to_bytes(bits);
    //     pi_.set_key(bytes.data(), bytes.size());
    //     return *this;
    // }

    PeerInfoBuilder& key(const Id& id) {
        auto bytes = to_bytes(id);
        pi_.set_key(bytes.data(), bytes.size());
        return *this;
    }

    PeerInfoBuilder& address(const boost::asio::ip::address& address) {
        auto ip_bytes = address.to_v4().to_bytes();
        pi_.set_addr(ip_bytes.data(), ip_bytes.size());
        return *this;
    }

    PeerInfoBuilder& port(uint32_t port) {
        pi_.set_port(port);
        return *this;
    }

    PeerInfoProto build() { return pi_; }
};

class MessageBuilder {
   private:
    Message msg_;

   public:
    MessageBuilder& type(MessageType type) {
        msg_.set_type(type);
        return *this;
    }

    MessageBuilder& from(const PeerInfo& user) {
        PeerInfoProto* info = msg_.mutable_from_user();
        *info               = PeerInfoBuilder()
                    .key(user.key_)
                    .address(user.endpoint_.address())
                    .port(user.endpoint_.port())
                    .build();
        return *this;
    }

    MessageBuilder& to(const PeerInfo& user) {
        PeerInfoProto* info = msg_.mutable_to_user();
        *info               = PeerInfoBuilder()
                    .key(user.key_)
                    .address(user.endpoint_.address())
                    .port(user.endpoint_.port())
                    .build();
        return *this;
    }

    MessageBuilder& find(const std::bitset<kIdLength>& id) {
        auto bytes = to_bytes(id);
        msg_.set_find_user(bytes.data(), bytes.size());
        return *this;
    }

    MessageBuilder& find(const Id& id) {
        auto bytes = to_bytes(id);
        msg_.set_find_user(bytes.data(), bytes.size());
        return *this;
    }

    MessageBuilder& result(const std::vector<PeerInfo>& peers) {
        for (const PeerInfo& peer : peers) {
            PeerInfoProto proto = PeerInfoBuilder()
                                      .key(peer.key_)
                                      .address(peer.endpoint_.address())
                                      .port(peer.endpoint_.port())
                                      .build();

            *msg_.add_result() = std::move(proto);
        }
        return *this;
    }

    MessageBuilder& nonce(uint64_t nonce) {
        msg_.set_nonce(nonce);
        return *this;
    }

    MessageBuilder& booted(bool successfulBoot) {
        msg_.set_bootstrapped(successfulBoot);
        return *this;
    }

    Message build() {
        msg_.set_timestamp(get_current_timestamp());
        return msg_;
    }
};

struct TimestampMs {
    int64_t value;
};

template <> struct fmt::formatter<TimestampMs> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const TimestampMs& ts, FormatContext& ctx) const {
        std::time_t t  = static_cast<std::time_t>(ts.value / 1000);
        std::tm     tm = *std::localtime(&t);
        return fmt::format_to(ctx.out(), "{:%Y-%m-%d %H:%M:%S}", tm);
    }
};
