#include "node.h"
#include "kademlia.h"
#include "swarm.h"

int Node::distance(const INode& node)
{
    return m_id.distance(node.getId());
}

void Node::bootstrap(Id& bootstrapId)
{
    insert(bootstrapId);
    Kademlia k;
    auto sth =  k.lookup(*this, *this);
    k.findNode(*this, *this, sth);
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

std::vector<std::shared_ptr<INode>> Node::copyTo(int bucketNumber, std::vector<std::shared_ptr<INode>>& result)
{
    Bucket b = m_buckets[bucketNumber];
    for(auto& id : b.getValue())
    {
        result.push_back(std::make_shared<Node>(Swarm::getInstance().getNode(id)));
    }
    return result;
}

const Id& Node::getId() const
{
    return m_id;
}

std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Id: " << node.m_id;
    return os;
}

bool Node::operator< (const INode& r) const
{
    return m_id < r.getId();
}

bool Node::operator== (const INode& r) const
{
    return m_id == r.getId();
}

bool Node::operator!= (const INode& r) const
{
    return m_id != r.getId();
}
