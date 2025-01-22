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

    bool findNode(INode& sender, INode& target, std::vector<std::shared_ptr<INode> >& pool);

    std::vector<std::shared_ptr<INode>> lookup(INode& node, INode& target);
};
