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

public:
    Node() = default;
    Kademlia kademlia;

    int distance(const Node& node);

    void bootstrap(Id& bootstrapId);

    bool remove(const Id& id);
    bool insert(const Id& id);

    const Id& getId() const {
        return m_id;
    }

    Bucket& getBucket(int bucketNumber) {
        try
        {
            return m_buckets.at(bucketNumber);
        }
        catch(const std::out_of_range& ex) {}
    }

    bool friend operator<(const Node& l, const Node& r);
    friend std::ostream& operator<<(std::ostream& os, const Node& node);
};
