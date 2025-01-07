#include "node.h"

int Node::distance(const Node& node)
{
    return m_id.distance(node.m_id);
}

void Node::bootstrap(Node& bootstrapNode)
{
    store(bootstrapNode.getId());
    std::vector<Id> ids = findNode(m_id, bootstrapNode.getId(), m_id);
}


bool Node::remove(const Id& id)
{
    return m_buckets[m_id.distance(id)].remove(id);
}


std::vector<Id> Node::findNode(const Id& sender, const Id& recipient, const Id& target)
{
    std::vector<Id> reply;

    Bucket bucket = m_buckets[m_id.distance(target)];

    for (int i = 0; i < gReplySize || bucket.getValue()[i] < gReplySize; ++i)
    {
        reply.push_back(bucket.getValue()[i]);
    }

    replyToFindNode(m_id, sender, reply);
}

bool Node::store(const Id& id)
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
