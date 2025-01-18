#pragma once

#include "bucket.h"
#include "id.h"
#include "kademlia.h"
#include "packetcounter.h"
#include <map>

class Node
{
private:
    Id m_id;
    std::map<int, Bucket> m_buckets;
    PacketCounter m_packetCounter;
    bool m_wasQueried = false;

public:
    Kademlia kademlia;

    Node(const Node& node)
        : m_id(node.m_id)
        , m_buckets(node.m_buckets)
        , m_packetCounter(node.m_packetCounter)
        {};
    Node() = default;

    int distance(const Node& node);

    void bootstrap(Id& bootstrapId);

    bool remove(const Id& id);
    bool insert(const Id& id);

    const Id& getId() const {
        return m_id;
    }

    void setQueried();
    bool queried();

    Bucket& getBucket(int bucketNumber);

    friend std::ostream& operator<<(std::ostream& os, const Node& node);
    friend bool operator<(const Node& l, const Node& r);
    friend bool operator==(const Node& l, const Node& r);
    friend bool operator!=(const Node& l, const Node& r);
};
