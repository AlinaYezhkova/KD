#pragma once

#include "inode.h"

namespace kademlia {

    void ping(INode& id, INode& target);
    void store(Id& id, Id& target);
    void findValue();

    bool findNode(INode& sender, INode& target);
    void lookup(INode& sender, INode& intermed, INode& target);
}
