#pragma once

#include "id.h"
class Node;

class Kademlia
{    
public:
    Kademlia() = default;

    void ping(Node& id, Node& target);
    void store(Id& id, Id& target);
    void findValue();

    // std::vector<Id> findNode(Id& sender, Id& receiver, Id& target);
    bool findNode(Node& target, std::vector<Node>& knownNodes);
    void replyToFindNode(const Id& sender, const Id& receiver, std::vector<Id> reply);

private:
    std::vector<Node> lookup(Node& node, Node& target);
};
