#include "id.h"
#include <cassert>
#include <iostream>

Id::Id() {}

Id::Id(int length) : m_length(length)
{
    generate();
}

Id::Id(int length, int seed) : m_length(length), m_seed(seed)
{
    generate(seed);
}


std::ostream& operator<<(std::ostream& os, const Id& id)
{
    for(bool b : id.m_value)
    {
        os << b << ' ';
    }
    os << std::endl;
    return os;
}

void Id::generate(std::optional<int> seed)
{
    m_value.resize(m_length);
    std::bernoulli_distribution dist(0.5); // Equal probability for true (0.5)
    std::mt19937 gen(seed.value_or(m_randomDevice()));
    for(int i = 0; i < m_length; ++i)
    {
        bool value = dist(gen);
        m_value[i] = value;
    }
}


int Id::distance(const Id& id)
{
    int result = 0;
    assert(m_length == id.m_length);

    for(; result < m_length && m_value[result] == id.m_value[result]; ++result);

    return result;
}

void Id::testId()
{
    Id id1(10, 0);
    std::cout << id1;

    Id id2(10, 0);
    std::cout << id2;

    Id id3(20, 1);
    std::cout << id3;

    Id id4(20, 0);
    std::cout << id4;

    std::cout << id1.distance(id2) << std::endl;
    std::cout << id3.distance(id4) << std::endl;
}
