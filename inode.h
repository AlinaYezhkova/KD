#pragma once

#include "fmt/base.h"
#include <memory>

using Id     = uint64_t;
using Bucket = std::vector<Id>;
class Pool;

class INode : public std::enable_shared_from_this<INode> {
   public:
    virtual int       distance(const Id& id) const              = 0;
    virtual void      bootstrap(Id& bootstrapId)                = 0;
    virtual void      remove(const Id& id)                      = 0;
    virtual void      insert(const Id& id)                      = 0;
    virtual Bucket&   getBucket(int bucketNumber)               = 0;
    virtual Pool&     getPool()                                 = 0;
    virtual const Id& getId() const                             = 0;
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
