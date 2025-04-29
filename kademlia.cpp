#include "kademlia.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <memory>

bool kademlia::findNode(INode& sender, INode& target, Pool<std::shared_ptr<INode> >& pool)
{
    LOG("=============================================================================")
    LOG("sender node: " << sender)
    LOG("target node: " << target)
    // Sort known nodes by distance to the target
    std::sort(pool.begin(), pool.end(), [&target](std::shared_ptr<INode>& a, std::shared_ptr<INode>& b) {
        return a->distance(target) < b->distance(target);
    });
    // TODO: here you can erase farthest pool nodes (the least necessary)
    LOG("pool after sorting: ")
    for(auto e : pool)
    {
        LOG(e->getId())
    }
    // Check if the target node is already in the known nodes
    for (auto& node : pool)
    {
        if (*node == target)
        {
            LOG("Target node found")
            sender.reset();
            return true;
        }
    }
    // Select up to alpha closest unqueried nodes
    LOG("Selecting up to alpha closest unqueried nodes.....")
    std::vector<std::shared_ptr<INode>> toQuery;
    for (auto node : pool)
    {
        // if (!node->queried() && toQuery.size() < gReaddressNumber)
        if (!sender.hasQueried(node) && toQuery.size() < gSpreadNumber)
        {
            toQuery.push_back(node);
            node->insert(sender.getId());
            sender.insert(node->getId());
        }
    }
    LOG("toQuery: ")
    for(auto e : toQuery)
    {
        LOG(*e)
    }
    // Base case: If no nodes to query or no progress, stop
    if (toQuery.empty())
    {
        std::cout << "Target node not found\n";
        sender.reset();
        return false;
    }

    // Mark nodes as queried and query them
    LOG("Marking nodes as queried and query them.....")
    for (std::shared_ptr<INode> node : toQuery) {
        sender.addToQueried(node);
        LOG("node " << node->getId() << " has been set as queried")
        // Query the node
        Pool<std::shared_ptr<INode>> closerNodes = lookup(*node, target);
        LOG("closerNodes: ")
        for(auto e : closerNodes)
        {
            LOG(*e)
        }
        // Add new nodes to knownNodes if they are not already in the list
        for (auto newNode : closerNodes)
        {
            if (std::find(pool.begin(), pool.end(), newNode) == pool.end()) // it is brand new
            {
                pool.push(newNode);
            }
        }
    }
    // Recursive call to continue the search
    LOG("Recursive call (sender: " << sender.getId() << ")....")
    return findNode(sender, target, pool);
}

void kademlia::store(Id& id, Id& target)
{
    // Swarm::getInstance().getNode(id).insert(target);
}

Pool<std::shared_ptr<INode>> kademlia::lookup(INode& node, INode& target)
{
    LOG("lookup!!!!!!!!")
    LOG("node: " << node << ", target: " << target)
    Pool<std::shared_ptr<INode>> result;
    int bucketNumber = node.distance(target);

    for(int j = bucketNumber; j > 0; --j) // closer ones
    {
        node.copyTo(j, result);
    }

    for(int k = bucketNumber+1; k < gIdLength; ++k) // farther ones
    {
        node.copyTo(k, result);
    }
    LOG("result of lookup (for potential query):")
    for(auto e : result)
    {
        LOG(*e)
    }
    LOG("end lookup!!!!!!!!")
    return result;
}
