#pragma once

#include "id.h"
#include "inode.h"
#include "pool.h"

namespace kademlia {

    void ping(INode& id, INode& target);
    void store(Id& id, Id& target);
    void findValue();

    bool findNode(INode& sender, INode& target, Pool<std::shared_ptr<INode> >& pool);

    Pool<std::shared_ptr<INode> > lookup(INode& node, INode& target);
}
