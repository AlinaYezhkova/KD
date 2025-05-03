#include "swarm.h"
#include "constants.h"
#include <climits>
#include <iostream>

Swarm::Swarm()
{
    unsigned int seed = 42;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    while(m_value.size() < gSwarmSize)
    {
        int random_number = dist(rng);
        Id id(random_number);
        Node n(id);
        m_value.insert({id, std::make_shared<Node>(n)});
    }
}

Swarm& Swarm::getInstance()
{
    static Swarm instance;
    return instance;
}

std::ostream& operator<<(std::ostream& os, const Swarm& swarm)
{
    os << "Swarm size: " << swarm.m_value.size()
       << " (out of " << pow(2, gIdLength) << ") " << std::endl
       << "Boot Node: " << swarm.m_value.begin()->first << std::endl;

    // for(auto& e : swarm)
    // {
    //     os << e.second->getId() << std::endl;
    // }
    return os;
}
