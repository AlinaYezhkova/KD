#pragma once

#include "fmt/base.h"
#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

using Id = uint64_t;
class Bucket;
class Pool;

class INode : public std::enable_shared_from_this<INode> {
   public:
    std::function<std::shared_ptr<INode>(Id)> getNode;

    virtual const Id&                getId() const = 0;
    virtual boost::asio::io_context& getContext()  = 0;
    virtual const boost::asio::strand<boost::asio::io_context::executor_type>&
                                                  getStrand() const = 0;
    virtual boost::asio::steady_timer&            getTimer()        = 0;
    virtual std::chrono::steady_clock::time_point getLastSeen()     = 0;
    virtual std::unordered_map<int, Bucket>&      getBuckets()      = 0;

    virtual int  commonPrefix(Id id) const                = 0;
    virtual void insertNode(Id id)                        = 0;
    virtual void asyncInsertNode(Id id)                   = 0;
    virtual void bootstrap(const Id& bootstrapId)         = 0;
    virtual void asyncPerformNodeLookup(const Id& target) = 0;
    virtual void asyncFindNode(const Id& target, std::function<void(bool)> callback)          = 0;
    virtual void sendFindNodeRPC(
        const Id&                            senderId,
        const Id&                            targetId,
        std::function<void(std::vector<Id>)> callback) = 0;
    virtual void            scheduleRefresh()          = 0;
    virtual void            refreshBuckets()           = 0;
    virtual std::vector<Id> getClosestKnownNodes(
        const Id& targetId) = 0;  // TODO: std::vector<Id> & ?
    virtual void print() = 0;
    friend bool operator<(const INode& l, const INode& r);
    friend bool operator==(const INode& l, const INode& r);
    friend bool operator!=(const INode& l, const INode& r);

    virtual ~INode() = default;
};

template <> struct fmt::formatter<INode> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const INode& node, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", node.getId());
    }
};

class Bucket {
    std::vector<Id> ids_;
    // Needed to look up lastSeen
    std::function<std::shared_ptr<INode>(const Id&)> getNode;

   public:
    void setGetNodeCallback(
        std::function<std::shared_ptr<INode>(const Id&)> cb);

    void insert(const Id& id);

    bool empty() const { return ids_.empty(); }
    bool contains(Id id) {
        return std::find(ids_.begin(), ids_.end(), id) != ids_.end();
    }

    auto begin() const { return ids_.begin(); }
    auto end() const { return ids_.end(); }

    Id randomIdInRange();
};
