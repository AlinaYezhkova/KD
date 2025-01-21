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
    auto bootNode = swarm.getNode(bootstrapId);
    if(bootNode.has_value())
    {
        bootNode.value()->insert(m_id);
#ifdef DEBUG
        std::cout << "bootstrap node: " << std::endl;
        std::cout << *bootNode.value() << '(' << bootNode.value() << ')' << std::endl;
#endif
    }
    Kademlia k;
    auto pool =  k.lookup(*this, *this);
    k.findNode(*this, *this, pool);
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
}

std::vector<std::shared_ptr<INode>>& Node::copyTo(int bucketNumber, std::vector<std::shared_ptr<INode>>& result)
{
#ifdef DEBUG
    std::cout << "copyTo....\n";
    std::cout << "bucketNumber: " << bucketNumber << std::endl;
#endif
    for(auto& id : m_buckets[bucketNumber])
    {
        auto node = Swarm::getInstance().getNode(id);

        if(node.has_value())
        {
#ifdef DEBUG
            std::cout << "attention!!!! 'result' will get " << *node.value() << '(' << node.value() << ')' << std::endl;
            std::cout << "end copyTo....\n";
#endif
            result.push_back(node.value());
        }
    }
    return result;
}

const Id& Node::getId() const
{
    return m_id;
}

void Node::print(std::ostream& os) const
{
    os << "Id: " << m_id << std::endl;
    os << "Buckets: " << std::endl;
    for(int i = 0; i < m_buckets.size(); ++i)
    {
        if(!m_buckets[i].empty())
        {
            os << i << " - " << m_buckets[i];
        }
    }
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
