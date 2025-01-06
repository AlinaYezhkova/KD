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

public:
    Node() = default;

    int distance(const Node& node);

    bool friend operator<(const Node& l, const Node& r);
    friend std::ostream& operator<<(std::ostream& os, const Node& node);
};
