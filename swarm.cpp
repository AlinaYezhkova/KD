#include "swarm.h"
#include "constants.h"
#include <climits>
#include <iostream>

Swarm::Swarm()
{
    unsigned int seed = 42;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    while (value_.size() < g_swarm_size) {
        int random_number = dist(rng);
        Id id(random_number);
        Node n(id);
        value_.insert({id, std::make_shared<Node>(n)});
    }
}

Swarm& Swarm::getInstance()
{
    static Swarm instance;
    return instance;
}

std::ostream& operator<<(std::ostream& os, const Swarm& swarm)
{
    os << "Swarm size: " << swarm.value_.size() << " (out of "
       << pow(2, g_id_length) << ") " << std::endl;
    return os;
}
