#include "node.h"
#include "kademlia.h"
#include "swarm.h"
#include <iostream>


Node::Node()
{
    m_buckets.resize(gIdLength+1);
}

int Node::distance(const INode& node)
{
    return m_id.distance(node.getId());
}

void Node::bootstrap(Id& bootstrapId)
{
    insert(bootstrapId);
    Swarm& swarm = Swarm::getInstance();
    // auto bootNode = swarm.getNode(bootstrapId);
    // bootNode->insert(m_id);
    auto pool =  kademlia::lookup(*this, *this);
    kademlia::findNode(*this, *this, pool);
}

bool Node::remove(const Id& id)
{
    return m_buckets[m_id.distance(id)].remove(id);
}

bool Node::insert(const Id& id)
{
    int dist = m_id.distance(id);
    if(dist == 0)
    {
        return false;
    }
    return m_buckets[dist].insert(id);
}

Bucket& Node::getBucket(int bucketNumber)
{
    return m_buckets[bucketNumber];
}

Pool<std::shared_ptr<INode>>& Node::copyTo(int bucketNumber, Pool<std::shared_ptr<INode>>& result)
{
    for(auto& id : m_buckets[bucketNumber])
    {
        result.push(Swarm::getInstance().getNode(id));
    }
    return result;
}

const Id& Node::getId() const
{
    return m_id;
}

void Node::print(std::ostream& os) const
{
    os << "Id: " << std::hex << m_id << std::endl;
    os << "Buckets: " << std::endl;
    for(int i = 0; i < m_buckets.size(); ++i)
    {
        if(!m_buckets[i].empty())
        {
            os << i << " - " << m_buckets[i];
        }
    }
}

void Node::reset()
{
    m_queried.clear();
}
bool Node::addToQueried(std::shared_ptr<INode> node)
{
    m_queried.push_back(node);
}
bool Node::hasQueried(std::shared_ptr<INode> node)
{
    return std::find(m_queried.begin(), m_queried.end(), node) == m_queried.end() ? false : true;
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
