#pragma once

#include "bucket.h"
#include "id.h"
#include "inode.h"
#include "packetcounter.h"

class Node : public INode
{
private:
    Id m_id;
    std::vector<Bucket> m_buckets;
    PacketCounter m_packetCounter;

public:
    Node(const Node& node)
        : m_id(node.m_id)
        , m_buckets(node.m_buckets)
        , m_packetCounter(node.m_packetCounter)
        {};
    Node();

    int distance(const INode& node) override;
    void bootstrap(Id& bootstrapId) override;
    bool remove(const Id& id) override;
    bool insert(const Id& id) override;
    Bucket& getBucket(int bucketNumber) override;
    std::vector<std::shared_ptr<INode> >& copyTo(int bucketNumber, std::vector<std::shared_ptr<INode> >& result) override;
    const Id& getId() const override;
    void print(std::ostream& os) const override;

    bool operator<(const INode& r) const override;
    bool operator==(const INode& r) const override;
    bool operator!=(const INode& r) const override;
};
