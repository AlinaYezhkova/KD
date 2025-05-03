#include "test.h"
#include "id.h"
#include "pool.h"
#include "swarm.h"
#include "utils.h"
#include <cassert>
#include <iostream>

void test::id()
{
    Id a{{1,1,1,1,1,1}};
    Id b{{1,1,1,1,1,0}};
    Id c{{1,1,1,1,0,0}};
    Id d{{1,1,1,0,0,0}};
    Id e{{1,1,0,0,0,0}};
    Id f{{1,0,0,0,0,0}};
    Id g{{0,0,0,0,0,0}};

    assert(a.distance(a) == 0);
    assert(b < a);
    assert(c < a);
    assert(d < a);
    assert(g < a);

    assert(a.distance(b) == 1);
    assert(a.distance(c) == 2);
    assert(a.distance(d) == 3);
    assert(a.distance(e) == 4);
    assert(a.distance(f) == 5);
    assert(a.distance(g) == 6);
}

void test::SwarmFindRandomNode()
{
    Swarm& swarm = Swarm::getInstance();
    auto it = swarm.begin();
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int>  distr(0, swarm.size());
    for(auto& e : swarm)
    {
        std::advance(it, distr(generator));
        // Node targetNode = *(it->second);
        // e.second.kademlia.findNode(e.first, targetNode.getId());

    }
}

void test::SwarmBootstrap()
{
    Swarm& swarm = Swarm::getInstance();
    LOG(swarm)
    Id bootstrapNodeId = swarm.begin()->first;

    // bootstrap node does not bootstrp itself
    auto it = swarm.begin();
    ++it;
    int i = 0;

    for (; it != swarm.end(); ++it, ++i)
    {
        it->second->bootstrap(bootstrapNodeId);
        if(i >= gBootNumber)
        {
            break;
        }
    }
}

void test::NodeCopy()
{
}

void test::swarmSearch()
{
    Swarm& swarm = Swarm::getInstance();
    Id bootstrapNodeId = swarm.begin()->first;

    auto sth = swarm.getNode(bootstrapNodeId);
    std::cout << sth;

}

void test::poolTest()
{
}
