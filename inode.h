#pragma once

#include "bucket.h"
#include "id.h"
#include <memory>

class INode
{
public:
    virtual int distance(const INode& node) = 0;
    virtual void bootstrap(Id& bootstrapId) = 0;
    virtual bool remove(const Id& id) = 0;
    virtual bool insert(const Id& id) = 0;
    virtual void setQueried() = 0;
    virtual bool queried() = 0;
    virtual Bucket& getBucket(int bucketNumber) = 0;
    virtual std::vector<std::shared_ptr<INode> > copyTo(int bucketNumber, std::vector<std::shared_ptr<INode>>& result) = 0;
    virtual const Id& getId() const = 0;
    virtual bool operator<(const INode& r) const = 0;
    virtual bool operator==(const INode& r) const = 0;
    virtual bool operator!=(const INode& r) const = 0;

    virtual ~INode() = default;
};
