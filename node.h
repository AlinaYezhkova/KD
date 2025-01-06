#pragma once

#include "bucket.h"
#include "id.h"
#include "packetcounter.h"

class Node
{
private:
    Id m_id;
    Bucket m_bucket;
    PacketCounter m_packetCounter;
    bool m_isBootstrap = false;

public:
    Node() = default;

    int distance(const Node& node);

    void setBootstrap() {
        m_isBootstrap = true;
    }

    const Id& getId() const {
        return m_id;
    }

    bool friend operator<(const Node& l, const Node& r);
    friend std::ostream& operator<<(std::ostream& os, const Node& node);
};
