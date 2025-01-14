#include "kademlia.h"
#include "swarm.h"


std::vector<Id> Kademlia::findNode(Id& sender, Id& receiver, Id& target) // я, bootstrap, меня.
{











    if(receiver == target)
    {
        return {target};
    }

    Node receiverNode = Swarm::getInstance().getNode(receiver);
    receiverNode.insert(sender);
    int bucketNumber = receiver.distance(target);
    return lookup(receiverNode, bucketNumber); // порыться у receiverNode в bucketNumber
}

void Kademlia::store(Id& id, Id& target)
{
    // Swarm::getInstance().getNode(id).insert(target);
}


//-----------------------------------------------------------------------------------------------------------

void Kademlia::fill(Node& node, int bucketNumber, std::vector<Id>& result)
{
    node.getBucket(bucketNumber).copyTo(result);
}

std::vector<Id> Kademlia::lookup(Node& node, int bucketNumber)
{
    std::vector<Id> result;

    for(int j = bucketNumber; j > 0 && result.size() < gBucketSize; --j) // closer ones
    {
        fill(node, j, result);
    }

    for(int k = bucketNumber+1; k < gIdLength && result.size() < gBucketSize; ++k) // farther ones
    {
        fill(node, k, result);
    }
}
