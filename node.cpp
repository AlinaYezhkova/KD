#include "node.h"

int Node::distance(const Node& node)
{
    return m_id.distance(node.m_id);
}

void Node::bootstrap(Id& bootstrapId)
{
    insert(bootstrapId);
    Bucket bucket = getBucket(distance(*this));
    kademlia.findNode(*this, );
}


bool Node::remove(const Id& id)
{
    return m_buckets[m_id.distance(id)].remove(id);
}

bool Node::insert(const Id& id)
{
    return m_buckets[m_id.distance(id)].insert(id);
}

void Node::setQueried()
{
    m_wasQueried = true;
}

bool Node::queried()
{
    return m_wasQueried;
}

Bucket& Node::getBucket(int bucketNumber)
{
    return m_buckets[bucketNumber];
    // try
    // {
    //     return m_buckets.at(bucketNumber);
    // }
    // catch(const std::out_of_range& ex) {}
}

std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Id: " << node.m_id;
    return os;
}

bool operator< (const Node& l, const Node& r)
{
    return l.m_id < r.m_id;
}

bool operator== (const Node& l, const Node& r)
{
    return l.m_id == r.m_id;
}

bool operator!= (const Node& l, const Node& r)
{
    return l.m_id != r.m_id;
}
