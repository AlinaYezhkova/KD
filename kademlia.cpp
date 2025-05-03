#include "kademlia.h"
#include "utils.h"
#include "swarm.h"
#include <iostream>
#include <memory>

bool kademlia::findNode(INode& sender, INode& target)
{
    LOG("======================================findNode=======================================")
    LOG("sender node: " << sender)
    LOG("target node: " << target)

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
        if (!sender.hasQueried(node) && queried < gAlpha)
        {
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
        std::cout << "sender: " << sender.getId() << ", target: " << target.getId() << std::endl;
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
    LOG("lookup!!!!!!!! " << sender.getId() << " queries " << intermed.getId() << " for " << target.getId())
    int bucketNumber = intermed.distance(target);

    for(int j = bucketNumber; j > 0; --j) // closer ones
    {
        for(auto e : intermed.getBucket(j))
        {
            sender.getPool().insert(Swarm::getInstance().getNode(e));
        }
    }

    for(int k = bucketNumber+1; k <= gIdLength; ++k) // farther ones
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
            LOG(e->getId())
        }
    }
    LOG("end lookup!!!!!!!!")
}
