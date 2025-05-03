#include "node.h"
#include "kademlia.h"
#include "swarm.h"
#include "utils.h"
#include <iostream>


Node::Node(const Id& id) : m_id(id), m_pool(id)
{
    m_buckets.resize(gIdLength+1);
}

int Node::distance(const INode& node)
{
    return m_id.distance(node.getId());
}

void Node::bootstrap(Id& bootstrapId)
{
    // node "knows" bootstrap node beforehand
    insert(bootstrapId);
    LOG(m_id << " bootstrapping .... ")
    kademlia::lookup(*this, *this, *this);
    kademlia::findNode(*this, *this);
}

void Node::remove(const Id& id)
{
    m_buckets[m_id.distance(id)].remove(id);
}

void Node::insert(const Id& id)
{
    int dist = m_id.distance(id);
    if(dist > 0)
    {
        m_buckets[dist].insert(id);
    }
}

Bucket& Node::getBucket(int bucketNumber)
{
    return m_buckets[bucketNumber];
}

Pool& Node::getPool()
{
    return m_pool;
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
    // os << "Pool (" << m_pool.getId() << "): " << std::endl;
    // for(auto& e : m_pool)
    // {
    //     os << e->getId() << std::endl;
    // }
}

void Node::reset()
{
    m_queried.clear();
    m_pool.clear();
}

bool Node::addToQueried(std::shared_ptr<INode> node)
{
    m_queried.insert(node);
}

bool Node::hasQueried(std::shared_ptr<INode> node)
{
    return m_queried.find(node) !=  m_queried.end();
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
