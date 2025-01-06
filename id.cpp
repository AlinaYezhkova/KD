#include "id.h"
#include <cassert>
#include <iostream>

std::random_device Id::m_randomDevice;

Id::Id()
{
    generate();
}

Id::Id(int length) : m_length(length)
{
    generate();
}

Id::Id(int length, int seed) : m_length(length)//, m_seed(seed)
{
    generate(seed);
}

void Id::generate(std::optional<int> seed)
{
    m_value.resize(m_length);
    std::bernoulli_distribution dist(0.5); // Equal probability for true (0.5)
    std::mt19937 gen(seed.value_or( m_randomDevice()));
    for(int i = 0; i < m_length; ++i)
    {
        bool value = dist(gen);
        m_value[i] = value;
    }
}

int Id::distance(const Id& id) const
{
    assert(m_length == id.m_length);
    int result = 0;

    for(; result < m_length && m_value[result] == id.m_value[result]; ++result);

    return result;
}


std::ostream& operator<<(std::ostream& os, const Id& id)
{
    for(const bool& b : id.m_value)
    {
        os << b << ' ';
    }
    os << std::endl;
    return os;
}

bool operator<(const Id& l, const Id& r)
{
    int firstDiffBit = l.distance(r);
    return l.m_value[firstDiffBit] < r.m_value[firstDiffBit];
}

bool operator == (const Id& l, const Id& r)
{
    return l.distance(r) == l.m_length;
}
