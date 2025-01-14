#include "swarm.h"
#include "constants.h"
#include <iostream>

Swarm::Swarm()
{
    while(m_value.size() < gSwarmSize)
    {
        Node n;
        m_value[n.getId()] = n;
    }
}

Swarm& Swarm::getInstance()
{
    static Swarm instance;
    return instance;
}

Node& Swarm::getNode(const Id& id)
{
    try
    {
        return m_value.at(id);
    }
    catch (const std::out_of_range& ex) {}
}

void Swarm::bootstrap()
{
    Id bootstrapNodeId = m_value.begin()->first;

    for (auto& e : m_value)
    {
        e.second.bootstrap(bootstrapNodeId);
    }
}

std::ostream& operator<<(std::ostream& os, const Swarm& swarm)
{
    os << "Swarm size: " << swarm.m_value.size()
       << " (out of " << pow(2, gIdLength) << ") " << std::endl
       << "Nodes: " << std::endl;

    for(auto& e : swarm.m_value)
    {
        os << e.second.getId();
    }
    os << std::endl;

    return os;
}
