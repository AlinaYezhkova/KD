#pragma once

#include "bucket.h"
#include "id.h"
#include <memory>

class Pool;

class INode : public std::enable_shared_from_this<INode>
{
public:
    virtual int distance(const INode& node) = 0;
    virtual void bootstrap(Id& bootstrapId) = 0;
    virtual void remove(const Id& id) = 0;
    virtual void insert(const Id& id) = 0;
    virtual Bucket& getBucket(int bucketNumber) = 0;
    virtual Pool& getPool() = 0;
    virtual const Id& getId() const = 0;
    virtual void reset() = 0;
    virtual bool addToQueried(std::shared_ptr<INode> node) = 0;
    virtual bool hasQueried(std::shared_ptr<INode> node) = 0;

    virtual bool operator<(const INode& r) const = 0;
    virtual bool operator==(const INode& r) const = 0;
    virtual bool operator!=(const INode& r) const = 0;
    virtual void print(std::ostream& os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const INode& node) {
        node.print(os);
        return os;
    };

    virtual ~INode() = default;
};

// inline std::ostream& operator<<(std::ostream& os, const INode& node) {
//     node.print(os);
//     return os;
// }
