#include "swarm.h"
#include "constants.h"
#include <iostream>
#include <set>

Swarm::Swarm() {}

void Swarm::initiate()
{
    std::set<Node> nodes;
    while(nodes.size() < gSwarmSize)
    {
        nodes.emplace();
    }
    m_value.assign(nodes.begin(), nodes.end());
    m_value.begin()->setBootstrap();
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
