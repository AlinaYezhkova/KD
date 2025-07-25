#include "kademlia.h"
#include "utils.h"
#include "swarm.h"
#include <iostream>
#include <memory>

bool kademlia::findNode(INode& sender, INode& target)
{
    LOG("======================================findNode=======================================")
    // Check if the target node is already in the known nodes
    if(sender.getPool().contains(Swarm::getInstance().getNode(target.getId())))
    {
        LOG("Target node found")
        sender.insert(target.getId());
        target.insert(sender.getId());
        sender.reset();
        return true;
    }

    // Select up to alpha closest unqueried nodes
    LOG("Selecting up to alpha closest unqueried nodes.....")
    int queried = 0;
    for (auto node : sender.getPool())
    {
        if (!sender.hasQueried(node) && queried < g_alpha) {
            sender.addToQueried(node);
            sender.insert(node->getId());
            node->insert(sender.getId());

            // update pool - a closer node gives even closer nodes
            lookup(sender, *node, target);

            ++queried;
            return findNode(sender, target);
        }
    }
    if (queried == 0)
    {
        std::cout << "Target node not found\n";
        sender.reset();
        return false;
    }
}

void kademlia::store(Id& id, Id& target)
{
    // Swarm::getInstance().getNode(id).insert(target);
}

void kademlia::lookup(INode& sender, INode& intermed, INode& target)
{
    int bucket_number = intermed.distance(target.getId());

    for (int j = bucket_number; j > 0; --j)  // closer ones
    {
        for(auto e : intermed.getBucket(j))
        {
            sender.getPool().insert(Swarm::getInstance().getNode(e));
        }
    }

    for (int k = bucket_number + 1; k <= g_id_length; ++k)  // farther ones
    {
        for(auto e : sender.getBucket(k))
        {
            sender.getPool().insert(Swarm::getInstance().getNode(e));
        }
    }

    LOG("result of lookup (for potential query):")
    if(sender.getPool().isEmpty())
    {
        LOG("found no suitable nodes during lookup")
    }
    else
    {
        for(auto e : sender.getPool())
        {
            fmt::println("{}", e->getId());
        }
    }
    LOG("end lookup!!!!!!!!")
}
