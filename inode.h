#pragma once

#include "constants.h"
#include "fmt/base.h"
#include "id.h"
#include <boost/asio.hpp>
#include <memory>
#include <set>

using udp = boost::asio::ip::udp;

struct PeerInfo {
    Id            key_;
    udp::endpoint endpoint_;
    int64_t       last_seen_;
};

bool operator<(const PeerInfo& l, const PeerInfo& r);
bool operator==(const PeerInfo& l, const PeerInfo& r);

class INode : public std::enable_shared_from_this<INode> {
   public:
    virtual std::vector<PeerInfo> find_K_closest(Id target)  = 0;
    virtual bool                  insert(const PeerInfo& pi) = 0;
    virtual const Id&             get_id() const             = 0;

    friend bool operator<(const INode& l, const INode& r);

    virtual ~INode() = default;
};

template <> struct fmt::formatter<INode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const INode& node, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", node.get_id());
    }
};
