#include "swarm.h"
#include "constants.h"
#include <iostream>

Swarm::Swarm()
{
    while(m_value.size() < gSwarmSize)
    {
        Node n;
        m_value.insert({n.getId(), std::make_shared<Node>(n)});
    }
}

Swarm& Swarm::getInstance()
{
    static Swarm instance;
    return instance;
}

std::optional<std::shared_ptr<INode> > Swarm::getNode(const Id& id)
{
    try
    {
        return m_value.at(id);
    }
    catch (const std::out_of_range& ex)
    {
#ifdef DEBUG
        std::cout << "swarm search exception\n";
#endif
        return std::nullopt;
    }
}

std::ostream& operator<<(std::ostream& os, const Swarm& swarm)
{
    os << "Swarm size: " << swarm.m_value.size()
       << " (out of " << pow(2, gIdLength) << ") " << std::endl
       << "Nodes: " << std::endl;

    for(auto& e : swarm)
    {
        os << e.second->getId() << std::endl;
    }
    return os;
}
