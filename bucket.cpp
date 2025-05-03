#include "bucket.h"
#include <algorithm>
#include <iostream>
#include "utils.h"

void Bucket::insert(const Id& id)
{
    if(std::find(m_value.begin(), m_value.end(), id) != m_value.end()
        || m_value.size() == gBucketSize)
    {
        return;
    }
    m_value.push_back(id);
    // TODO: update last seen
}

void Bucket::remove(const Id& id)
{
    m_value.erase(std::find(m_value.begin(), m_value.end(), id));
}

std::ostream& operator<< (std::ostream& os, const Bucket& bucket)
{
    for(auto e : bucket.m_value)
    {
        os << e << std::endl;
    }
    return os;
}
