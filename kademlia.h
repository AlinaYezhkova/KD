#pragma once

#include "id.h"
#include "inode.h"

class Kademlia
{    
public:
    Kademlia() = default;

    void ping(INode& id, INode& target);
    void store(Id& id, Id& target);
    void findValue();

    // std::vector<Id> findNode(Id& sender, Id& receiver, Id& target);
    bool findNode(INode& sender, INode& target, std::vector<std::shared_ptr<INode> >& pool);
    void replyToFindNode(const Id& sender, const Id& receiver, std::vector<Id> reply);

private:
    std::vector<std::shared_ptr<INode>> lookup(INode& node, INode& target);
};
