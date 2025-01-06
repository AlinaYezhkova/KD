#include "swarm.h"
#include "constants.h"
#include <iostream>

Swarm::Swarm() {}

void Swarm::initiate()
{
    while(m_value.size() < gSwarmSize)
    {
        m_value.emplace();
    }
}

std::ostream& operator<<(std::ostream& os, const Swarm& swarm)
{
    os << "Swarm size: " << swarm.m_value.size() << std::endl << "Nodes: " << std::endl;
    for(const Node& n : swarm.m_value)
    {
        os << n;
    }
    os << std::endl;
    return os;
}
