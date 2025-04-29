#include "test.h"
#include "id.h"
#include "pool.h"
#include "swarm.h"
#include "utils.h"
#include <cassert>
#include <iostream>

void test::id()
{
    Id id1(10, 0);
    Id id2(10, 0);
    assert(id1 == id2);
    assert(id1.distance(id2) == 0);

    Id id3(10);
    std::cout << id3 << std::endl;

    Id id4(10);
    std::cout << id4 << std::endl;

    std::cout << id3.distance(id4) << std::endl;


    Id id5;
    assert(id5.getValue().size() == gIdLength);

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

    Id aa;
    Id ba(aa);
    assert(aa == ba);
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
    // LOG(swarm)
    Id bootstrapNodeId = swarm.begin()->first;
    auto it = swarm.begin();
    ++it;
    for (int i = 0; i < gBootNumber; ++i)
    {
        it->second->bootstrap(bootstrapNodeId);
        ++it;
    }
}

void test::NodeCopy()
{
    Node a;
    Node b(a);

    std::cout << a;
    std::cout << b;
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
    Pool<int> rb;

    rb.push(10);
    rb.push(20);
    rb.push(30);
    rb.push(40);
    rb.push(50);
    for(auto& e : rb)
    {
        std::cout << e << ' ';
    }
    std::cout << std::endl;

    rb.push(60);
    rb.push(70);

    for(auto& e : rb)
    {
        std::cout << e << ' ';
    }
    std::cout << std::endl;
}
