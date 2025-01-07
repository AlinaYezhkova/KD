#include "bucket.h"
#include <algorithm>

Bucket::Bucket() {};

bool Bucket::insert(const Id& id)
{
    if(m_value.size() == gBucketSize)
    {
        return false;
    }
    m_value.push_back(id);
    return true;
}

bool Bucket::remove(const Id& id)
{
    auto it = std::find(m_value.begin(), m_value.end(), id);
    if (it == m_value.end())
    {
        return false;
    }
    m_value.erase(it);
    return true;
}
