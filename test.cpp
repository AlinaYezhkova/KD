#include "test.h"
#include "id.h"
#include "swarm.h"
#include <iostream>

void test::node() {
    Node a(135836);
    Node b(255295);  // max 1048576
    std::cout << a.distance(b.getId()) << '\n';

    // 00100001001010011100
    // 00111110010100111111
    // 11110010010111000011
}

// void test::SwarmFindRandomNode()
// {
//     Swarm& swarm = Swarm::getInstance();
//     auto it = swarm.begin();
//     std::random_device rand_dev;
//     std::mt19937 generator(rand_dev());
//     std::uniform_int_distribution<int>  distr(0, swarm.size());
//     for(auto& e : swarm)
//     {
//         std::advance(it, distr(generator));
//         // Node targetNode = *(it->second);
//         // e.second.kademlia.findNode(e.first, targetNode.getId());

//     }
// }

// void test::SwarmBootstrap()
// {
//     Swarm& swarm = Swarm::getInstance();
//     LOG(swarm)
//     Id bootstrap_node_id = swarm.begin()->first;

//     // bootstrap node does not bootstrp itself
//     auto it = swarm.begin();
//     ++it;
//     int i = 0;

//     for (; it != swarm.end(); ++it, ++i)
//     {
//         it->second->bootstrap(bootstrap_node_id);
//         if (i >= g_boot_number) {
//             break;
//         }
//     }
// }

// void test::NodeCopy()
// {
// }

void test::swarmSearch()
{
    Swarm& swarm = Swarm::getInstance();
    Id     bootstrap_node_id = swarm.begin()->first;

    auto sth = swarm.getNode(bootstrap_node_id);
    std::cout << sth;

}

void test::poolTest()
{
}
