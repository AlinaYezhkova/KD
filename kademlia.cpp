#include "kademlia.h"
#include <algorithm>
#include <memory>

// std::vector<Id> Kademlia::findNode(Id& sender, Id& receiver, Id& target) // я, bootstrap, меня.
// {
//     int targetBucketNumber = sender.distance(target);
//     Node senderNode = Swarm::getInstance().getNode(sender);
//     Bucket targetBucket = senderNode.getBucket(targetBucketNumber);



//     // if(receiver == target)
//     // {
//     //     return {target};
//     // }


//     // Node receiverNode = router.getNode(receiver);
//     Node receiverNode = Swarm::getInstance().getNode(receiver);
//     receiverNode.insert(sender);
//     int bucketNumber = receiver.distance(target);
//     auto result =  lookup(receiverNode, bucketNumber); // порыться у receiverNode в bucketNumber
//     if(std::find(result.begin(), result.end(), target) == result.end())
//     {

//     }
//     return result;
// }


bool Kademlia::findNode(INode& sender, INode& target, std::vector<std::shared_ptr<INode>>& pool)
{
    // Sort known nodes by distance to the target
    std::sort(pool.begin(), pool.end(), [&](std::shared_ptr<INode>& a, std::shared_ptr<INode>& b) {
        return a->distance(target) < b->distance(target);
    });

    // TODO: here you can erase farthest pool nodes (the least necessary)

    // Check if the target node is already in the known nodes
    for (auto node : pool)
    {
        if (*node == target)
        {
            return true; // Target node found
        }
    }

    // Select up to alpha closest unqueried nodes
    std::vector<std::shared_ptr<INode>> toQuery;
    for (auto node : pool)
    {
        if (!node->queried() && toQuery.size() < gReaddressNumber)
        {
            toQuery.push_back(node);
        }
    }

    // Base case: If no nodes to query or no progress, stop
    if (toQuery.empty()) {
        return false; // Target node not found
    }

    // Mark nodes as queried and query them
    for (std::shared_ptr<INode> node : toQuery) {
        node->setQueried();

        // Query the node
        std::vector<std::shared_ptr<INode>> closerNodes = lookup(*node, target);

        // Add new nodes to knownNodes if they are not already in the list
        for (auto newNode : closerNodes)
        {
            if (std::find(pool.begin(), pool.end(), newNode) == pool.end()
                && pool.size() <= gFindNodeSize)
            {
                pool.push_back(newNode);
                sender.insert(newNode->getId());
            }
        }
    }

    // Recursive call to continue the search
    return findNode(sender, target, pool);
}


void Kademlia::store(Id& id, Id& target)
{
    // Swarm::getInstance().getNode(id).insert(target);
}

std::vector<std::shared_ptr<INode>> Kademlia::lookup(INode& node, INode& target)
{
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
    return result;
}
