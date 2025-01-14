#include "node.h"

int Node::distance(const Node& node)
{
    return m_id.distance(node.m_id);
}

void Node::bootstrap(Id& bootstrapId)
{
    kademlia.store(this->m_id, bootstrapId);
    std::vector<Id> ids = kademlia.findNode(this->m_id, bootstrapId, this->m_id);
}


bool Node::remove(const Id& id)
{
    return m_buckets[m_id.distance(id)].remove(id);
}

bool Node::insert(const Id& id)
{
    return m_buckets[m_id.distance(id)].insert(id);
}

bool operator<(const Node& l, const Node& r)
{
    return l.m_id < r.m_id;
}

std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Id: " << node.m_id;
    return os;
}
