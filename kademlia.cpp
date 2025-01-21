#include "kademlia.h"
#include <algorithm>
#include <iostream>
#include <memory>

bool Kademlia::findNode(INode& sender, INode& target, std::vector<std::shared_ptr<INode>>& pool)
{
#ifdef DEBUG
    std::cout << "=============================================================================" << std::endl;
    std::cout << "sender node: " << sender;
    std::cout << "target node: " << target;
    std::cout << "pool before sorting: " << std::endl;
    for(auto e : pool)
    {
        std::cout << e->getId() << std::endl;
    }
#endif
    // Sort known nodes by distance to the target
    std::sort(pool.begin(), pool.end(), [&](std::shared_ptr<INode>& a, std::shared_ptr<INode>& b) {
        return a->distance(target) < b->distance(target);
    });
    // TODO: here you can erase farthest pool nodes (the least necessary)
#ifdef DEBUG
    std::cout << "pool after sorting: " << std::endl;
    for(auto e : pool)
    {
        std::cout << e->getId() << std::endl;
    }
#endif
    // Check if the target node is already in the known nodes
    for (auto& node : pool)
    {
        if (*node == target)
        {
            std::cout << "Target node found\n";
            return true;
        }
    }
    // Select up to alpha closest unqueried nodes
#ifdef DEBUG
    std::cout << "Selecting up to alpha closest unqueried nodes.....\n";
#endif
    std::vector<std::shared_ptr<INode>> toQuery;
    for (auto node : pool)
    {
        if (!node->queried() && toQuery.size() < gReaddressNumber)
        {
            toQuery.push_back(node);
        }
    }
#ifdef DEBUG
    std::cout << "toQuery: " << std::endl;
    for(auto e : toQuery)
    {
        std::cout << *e << std::endl << '(' << e << ')' << std::endl;
    }
#endif
    // Base case: If no nodes to query or no progress, stop
    if (toQuery.empty())
    {
        std::cout << "Target node not found\n";
        return false;
    }

    // Mark nodes as queried and query them
#ifdef DEBUG
    std::cout << "Marking nodes as queried and query them.....\n";
#endif
    for (std::shared_ptr<INode> node : toQuery) {
        node->setQueried();
#ifdef DEBUG
        std::cout << "node " << node->getId() << " has been set as queried\n";
#endif
        // Query the node
        std::vector<std::shared_ptr<INode>> closerNodes = lookup(*node, target);
#ifdef DEBUG
        std::cout << "closerNodes: " << std::endl;
        for(auto e : closerNodes)
        {
            std::cout << *e << std::endl << '(' << e << ')' << std::endl;
        }
#endif
        // Add new nodes to knownNodes if they are not already in the list
        for (auto newNode : closerNodes)
        {
            if (std::find(pool.begin(), pool.end(), newNode) == pool.end()
                && pool.size() <= gFindNodeSize)
            {
                pool.push_back(newNode);
                newNode->insert(sender.getId());
                sender.insert(newNode->getId());
            }
        }
    }
    // Recursive call to continue the search
#ifdef DEBUG
    std::cout << "Recursive call (sender: " << sender.getId() << ")....\n";
#endif
    return findNode(sender, target, pool);
}

void Kademlia::store(Id& id, Id& target)
{
    // Swarm::getInstance().getNode(id).insert(target);
}

std::vector<std::shared_ptr<INode>> Kademlia::lookup(INode& node, INode& target)
{
#ifdef DEBUG
    std::cout << "lookup!!!!!!!!\n";
    std::cout << "node: " << node << ", target: " << target << std::endl;
#endif
    std::vector<std::shared_ptr<INode>> result;
    int bucketNumber = node.distance(target);

    for(int j = bucketNumber; j > 0 && result.size() < gFindNodeSize; --j) // closer ones
    {
        node.copyTo(j, result);
    }

    for(int k = bucketNumber+1; k < gIdLength && result.size() < gFindNodeSize; ++k) // farther ones
    {
        node.copyTo(k, result);
    }
#ifdef DEBUG
    std::cout << "result of lookup (for potential query): \n";
    for(auto e : result)
    {
        std::cout << *e << std::endl << '(' << e << ')' << std::endl;
    }
    std::cout << "end lookup!!!!!!!!\n";
#endif
    return result;
}
