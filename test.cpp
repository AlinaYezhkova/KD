#include "test.h"

#include "swarm.h"
#include "utils.h"
#include <iostream>

// void test::node() {
    // Node a(135836);
    // fmt::println("{}", toBinaryString(a.getId()));
    // Node b(255295);  // max 1048576
    // fmt::println("{}, {}", toBinaryString(b.getId()), a.commonPrefix(b));
    // auto d = a.distance(b.getId());
    // std::cout << d << '\n';
    // fmt::println("{}", toBinaryString(d));
    // fmt::println("");

    // Node a1(1358);
    // fmt::println("{}", toBinaryString(a1.getId()));
    // Node b1(5529);  // max 1048576
    // fmt::println("{}, {}", toBinaryString(b1.getId()), a1.commonPrefix(b1));
    // auto d1 = a1.distance(b1.getId());
    // std::cout << d1 << '\n';
    // fmt::println("{}", toBinaryString(d1));
    // fmt::println("");

    // Node a2(34);
    // fmt::println("{}", toBinaryString(a2.getId()));
    // Node b2(77);  // max 1048576
    // fmt::println("{}, {}", toBinaryString(b2.getId()), a2.commonPrefix(b2));
    // auto d2 = a2.distance(b2.getId());
    // std::cout << d2 << '\n';
    // fmt::println("{}", toBinaryString(d2));

    //     00100001001010011100
    //     00111110010100111111
    //     128931
    //     00011111011110100011
// }

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


