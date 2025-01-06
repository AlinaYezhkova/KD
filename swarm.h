#pragma once

#include "node.h"
#include <set>

class Swarm
{
private:
    std::set<Node> m_value;

public:
    Swarm();

    void initiate();

    friend std::ostream& operator<<(std::ostream& os, const Swarm& swarm);

};
