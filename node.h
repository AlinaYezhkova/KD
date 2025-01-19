#pragma once

#include "bucket.h"
#include "id.h"
#include "inode.h"
#include "packetcounter.h"
#include <map>

class Node : public INode
{
private:
    Id m_id;
    std::map<int, Bucket> m_buckets;
    PacketCounter m_packetCounter;
    bool m_wasQueried = false;

public:
    Node(const Node& node)
        : m_id(node.m_id)
        , m_buckets(node.m_buckets)
        , m_packetCounter(node.m_packetCounter)
        {};
    Node() = default;

    int distance(const INode& node) override;
    void bootstrap(Id& bootstrapId) override;
    bool remove(const Id& id) override;
    bool insert(const Id& id) override;
    void setQueried() override;
    bool queried() override;
    Bucket& getBucket(int bucketNumber) override;
    void copyTo(int bucketNumber, std::vector<std::shared_ptr<INode> >& result) override;
    const Id& getId() const override;

    friend std::ostream& operator<<(std::ostream& os, const Node& node);
    bool operator<(const INode& r) const override;
    bool operator==(const INode& r) const override;
    bool operator!=(const INode& r) const override;
};
