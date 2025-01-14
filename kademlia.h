#pragma once

#include "id.h"

// forward declaration
class Node;

class Kademlia
{    
public:
    Kademlia() = default;

    void ping(Node& id, Node& target);
    void store(Id& id, Id& target);
    void findValue();

    std::vector<Id> findNode(Id& sender, Id& receiver, Id& target);
    void replyToFindNode(const Id& sender, const Id& receiver, std::vector<Id> reply);

private:
    void fill(Node& node, int bucketNumber, std::vector<Id>& result);
    std::vector<Id> lookup(Node& node, int bucketNumber);
};
