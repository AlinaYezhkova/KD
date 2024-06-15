#pragma once

#include "Constants.h"
#include <QDebug>
#include <bitset>
#include <iostream>
#include <random>
#include <vector>
#include <cstdint>

class Id {

    std::vector<uint32_t> m_data;

public:
    Id()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, std::numeric_limits<uint32_t>::max());

        for (int i = 0; i < ID_LENGTH / (sizeof(uint32_t)*8); ++i) {
            m_data.push_back(dis(gen));
        }
    }

    Id(std::vector<uint32_t> v) : m_data(v) {}

    void printDataBinary()
    {
        for(auto& e : m_data)
        {
            std::cout << std::bitset<sizeof(uint32_t) * 8>(e) << " ";
        }
    }

    void printDataDecimal()
    {
        for(auto& e : m_data)
        {
            std::cout << e << " ";
        }
    }

    void printCheck()
    {
        for(auto& e : m_data)
        {
            std::cout << e << " " << std::bitset<sizeof(uint32_t) * 8>(e) << '\n';
        }
    }

    int commonPrefix(Id& id)
    {
        int bits = 0;
        for(int i = 0; i < m_data.size(); ++i)
        {
            auto a = std::bitset<sizeof(uint32_t) * 8>(m_data[i]);
            auto b = std::bitset<sizeof(uint32_t) * 8>(id.m_data[i]);
            for(int j = sizeof(uint32_t) * 8 - 1; j >= 0; --j)
            {
                if(a[j] == b[j])
                {
                    ++bits;
                }
                else
                {
                    return bits;
                }
            }
        }
        return bits;
    }


    std::vector<uint32_t> diffDecimal(const Id& id) const
    {
        std::vector<uint32_t> diffDec;
        for(int i = 0; i < ID_LENGTH / (sizeof(uint32_t)*8); ++i)
        {
            diffDec.push_back(m_data[i]^id.m_data[i]);
        }
        return diffDec;
    }

    void diffBinary(std::vector<uint32_t> v)
    {
        for(auto& e : v)
        {
            std::cout << std::bitset<sizeof(uint32_t) * 8>(e) << " ";
        }
    }
};
