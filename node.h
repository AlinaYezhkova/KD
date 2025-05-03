#pragma once

#include "bucket.h"
#include "id.h"
#include "inode.h"
#include "pool.h"
#include "packetcounter.h"
#include <algorithm>

class Node : public INode
{
private:
    Id m_id;
    std::vector<Bucket> m_buckets;
    std::set<std::shared_ptr<INode> > m_queried;
    Pool m_pool;
    PacketCounter m_packetCounter;

public:
    Node() = delete;
    Node(const Node& node)
        : m_id(node.m_id)
        , m_buckets(node.m_buckets)
        , m_pool(node.m_pool)
        , m_packetCounter(node.m_packetCounter)
        {};
    Node(const Id& id);

    int distance(const INode& node) override;
    void bootstrap(Id& bootstrapId) override;
    void remove(const Id& id) override;
    void insert(const Id& id) override;
    Bucket& getBucket(int bucketNumber) override;
    Pool& getPool() override;
    const Id& getId() const override;
    void print(std::ostream& os) const override;
    void reset() override;
    bool addToQueried(std::shared_ptr<INode> node) override;
    bool hasQueried(std::shared_ptr<INode> node) override;

    bool operator<(const INode& r) const override;
    bool operator==(const INode& r) const override;
    bool operator!=(const INode& r) const override;
};
