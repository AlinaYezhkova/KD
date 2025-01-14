#include "bucket.h"
#include <algorithm>

bool Bucket::insert(const Id& id)
{
    if(m_value.size() == gBucketSize)
    {
        return false;
    }
    m_value.push_back(id);
    // TODO: update last seen
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

std::vector<Id> Bucket::copyTo(std::vector<Id>& result)
{
    if(result.empty())
    {
        result.insert(result.begin(), m_value.begin(), m_value.end());
    }
    else
    {
        result.insert(result.end(), m_value.begin(), m_value.end());
    }
    return result;
}
