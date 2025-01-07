#pragma once

#include "bucket.h"
#include "id.h"
#include "packetcounter.h"
#include <map>

class Node
{
private:
    Id m_id;
    std::map<int, Bucket> m_buckets;
    PacketCounter m_packetCounter;
    bool m_isBootstrap = false;

public:
    Node() = default;

    int distance(const Node& node);

    void bootstrap();

    bool insert(const Id& id);
    bool remove(const Id& id);

    void ping();
    void store();
    void findNode();
    void findValue();

    void setBootstrap() {
        m_isBootstrap = true;
    }

    const Id& getId() const {
        return m_id;
    }

    bool friend operator<(const Node& l, const Node& r);
    friend std::ostream& operator<<(std::ostream& os, const Node& node);
};
