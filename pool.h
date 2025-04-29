#pragma once

#include "constants.h"
#include <deque>

template <typename T>
class Pool
{
private:
    std::deque<T> m_deque;

public:
    Pool() = default;
    void push(T element)
    {
        m_deque.push_front(element);

        if(m_deque.size() > gPoolSize)
        {
            m_deque.pop_back();
        }
    };

    auto begin() {return m_deque.begin();}
    auto end() {return m_deque.end();}

    auto begin() const {return m_deque.begin();}
    auto end() const {return m_deque.end();}

    auto cbegin() const {return m_deque.cbegin();}
    auto cend() const {return m_deque.cend();}

};
