#include "kademlia.h"
#include "node.h"
#include "swarm.h"
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


bool Kademlia::findNode(Node& target, std::vector<Node>& knownNodes)
{
    // Sort known nodes by distance to the target
    std::sort(knownNodes.begin(), knownNodes.end(), [&](Node a, Node b) {
        return a.distance(target) < b.distance(target);
    });

    // TODO: here you can erase farthest nodes (the least necessary)

    // Check if the target node is already in the known nodes
    for (Node& node : knownNodes) {
        if (node == target) {
            // if(knownNodes.size() > gBucketSize) {
            //     knownNodes.resize(gBucketSize);
            // }
            return true; // Target node found
        }
    }

    // Select up to alpha closest unqueried nodes
    std::vector<std::shared_ptr<Node>> toQuery;
    for (Node &node : knownNodes) {
        if (!node.queried() && toQuery.size() < gReaddressNumber) {
            toQuery.push_back(std::make_shared<Node>(node));
        }
    }

    // Base case: If no nodes to query or no progress, stop
    if (toQuery.empty()) {
        // if(knownNodes.size() > gBucketSize) {
        //     knownNodes.resize(gBucketSize);
        // }
        return false; // Target node not found
    }

    // Mark nodes as queried and query them
    for (std::shared_ptr<Node> node : toQuery) {
        node->setQueried();

        // Query the node
        std::vector<Node> closerNodes = lookup(*node, target);

        // Add new nodes to knownNodes if they are not already in the list
        for (Node& newNode : closerNodes)
        {
            if (std::find(knownNodes.begin(), knownNodes.end(), newNode) == knownNodes.end()
                && knownNodes.size() <= gFindNodeSize)
            {
                knownNodes.push_back(newNode);
                // TODO: this->insert(newNode);
            }
        }
    }

    // Recursive call to continue the search
    return findNode(target, knownNodes);
}


void Kademlia::store(Id& id, Id& target)
{
    // Swarm::getInstance().getNode(id).insert(target);
}


//-----------------------------------------------------------------------------------------------------------

std::vector<Node> Kademlia::lookup(Node& node, Node& target)
{
    std::vector<Node> result;
    std::vector<Id> ids;

    int bucketNumber = node.distance(target);

    for(int j = bucketNumber; j > 0 && result.size() < gBucketSize; --j) // closer ones
    {
        node.getBucket(bucketNumber).copyTo(ids);
    }

    for(int k = bucketNumber+1; k < gIdLength && result.size() < gBucketSize; ++k) // farther ones
    {
        node.getBucket(bucketNumber).copyTo(ids);
    }

    for(Id& id : ids)
    {
        result.push_back(Swarm::getInstance().getNode(id));
    }

    return result;
}
