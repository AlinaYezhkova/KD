#pragma once

#include "constants.h"
#include <optional>
#include <ostream>
#include <random>
#include <vector>

class Id
{
private:
    int m_length = gIdLength;
    std::vector<bool> m_value;

public:
    Id(const Id& id) : m_value(id.m_value) {};
    Id();
    Id(int length);
    Id(int length, int seed);
    Id(std::vector<bool> value);

    const std::vector<bool>& getValue() {
        return m_value;
    }

    int commonPrefix(const Id& id) const;
    int distance(const Id& id) const;

    friend std::ostream& operator<<(std::ostream& os, const Id& id);
    friend bool operator<(const Id& l, const Id& r);
    friend bool operator==(const Id& l, const Id& r);
    friend bool operator!=(const Id& l, const Id& r);

private:
    static std::random_device m_randomDevice;
    void generate(std::optional<int> seed = std::nullopt);
};

