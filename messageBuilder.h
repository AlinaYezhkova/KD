#pragma once

#include <fmt/chrono.h>
#include <fmt/core.h>

#include "message.pb.h"
#include <magic_enum/magic_enum.hpp>

/*
message PeerInfoProto {
    bytes key = 1;
    bytes addr = 2;    // must be 4 bytes
    uint32 port = 3;

    bytes name = 4;
}

message NodeIdProto {
    fixed64 high = 1;
    fixed64 low = 2;

    bytes name = 3;
}
*/

class NodeIdBuilder {
   private:
    NodeIdProto id_;

   public:
    NodeIdBuilder& high(uint64_t high) {
        id_.set_high(high);
        return *this;
    }

    NodeIdBuilder& low(uint64_t low) {
        id_.set_low(low);
        return *this;
    }

    NodeIdProto build() { return id_; }
};

class PeerInfoBuilder {
   private:
    PeerInfoProto pi_;

   public:
    PeerInfoBuilder& key(const std::array<uint64_t, 2>& key) {
        NodeIdProto* id = pi_.mutable_key();
        *id             = NodeIdBuilder().high(key[0]).low(key[1]).build();
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

    MessageBuilder& find(NodeId id) {
        NodeIdProto* tmp = msg_.mutable_find_user();
        *tmp             = NodeIdBuilder().high(id[0]).low(id[1]).build();
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

template <> struct fmt::formatter<NodeIdProto> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const NodeIdProto& id, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}{}", id.high(), id.low());
    }
};

// template <> struct fmt::formatter<Message> {
//     constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

//     template <typename FormatContext>
//     auto format(const Message& msg, FormatContext& ctx) const {
//         std::string result;
//         if (!msg.from_user().empty()) {
//             result += fmt::format(" - [{}]", msg.from_user());
//         }
//         if (!msg.to_user().empty()) {
//             result += fmt::format(" -> [{}]", msg.to_user());
//         }
//         if (!msg.find_user().empty()) {
//             result += fmt::format(" - {}", msg.find_user());
//         }
//         if (!result.empty()) {
//             result =
//                 fmt::format("{}", TimestampMs{msg.timestamp()}) + result +
//                 '\n';
//         }
//         return fmt::format_to(ctx.out(), "{}", result);
//     }
// };
