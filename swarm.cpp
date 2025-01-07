#include "swarm.h"
#include "constants.h"
#include <iostream>
#include <set>

Swarm::Swarm() {}

void Swarm::initiate()
{
    std::set<Node> nodes;
    while(m_value.size() < gSwarmSize)
    {
        nodes.emplace();
    }
    m_value.insert(m_value.begin(), nodes.begin(), nodes.end());
}

void Swarm::bootstrap()
{
    m_value.begin()->setBootstrap();

    for (Node& node : m_value)
    {
        node.bootstrap();
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
