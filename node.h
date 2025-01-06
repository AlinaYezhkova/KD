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
    Node();

    int distance(const Node& node);

};
