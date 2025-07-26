#pragma once

#include "constants.h"
#include "fmt/base.h"
#include <functional>
#include <memory>
#include <set>

using Id = uint64_t;
class Bucket;
class Pool;

class INode : public std::enable_shared_from_this<INode> {
   public:
    virtual int       distance(const Id& id) const              = 0;
    virtual void      bootstrap(std::shared_ptr<INode> node)    = 0;
    virtual void      remove(std::shared_ptr<INode>)            = 0;
    virtual void      insert(std::shared_ptr<INode>)            = 0;
    virtual Bucket&   getBucket(int bucketNumber)               = 0;
    virtual Pool&     getPool()                                 = 0;
    virtual const Id& getId() const                             = 0;
    virtual const std::chrono::time_point<std::chrono::system_clock>
                      getLastSeen() const                       = 0;
    virtual void      reset()                                   = 0;
    virtual bool      addToQueried(std::shared_ptr<INode> node) = 0;
    virtual bool      hasQueried(std::shared_ptr<INode> node)   = 0;

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
   private:
    using Comparator = std::function<bool(const std::shared_ptr<INode>&,
                                          const std::shared_ptr<INode>&)>;
    std::set<std::shared_ptr<INode>, Comparator> set_;

   public:
    Bucket();
    void insert(std::shared_ptr<INode> node);
    void erase(std::shared_ptr<INode> node);

    bool contains(const std::shared_ptr<INode>& node) {
        return set_.find(node) != set_.end();
    }
};
