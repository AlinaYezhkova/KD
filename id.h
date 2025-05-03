#pragma once

#include "constants.h"
#include <optional>
#include <ostream>
#include <random>

class Id
{
public:
    int m_length = gIdLength;
    std::vector<bool> m_value;
    void generate(std::optional<int> seed = std::nullopt);
private:
    static std::random_device m_randomDevice;

public:
    Id();
    Id(const Id& id) : m_value(id.m_value) {};
    Id(int seed);
    Id(const std::vector<bool>& value);

    int commonPrefix(const Id& id) const;
    int distance(const Id& id) const;

    friend std::ostream& operator<<(std::ostream& os, const Id& id);
    friend bool operator<(const Id& l, const Id& r);
    friend bool operator==(const Id& l, const Id& r);
    friend bool operator!=(const Id& l, const Id& r);
};

namespace std {
template <>
struct hash<Id> {
    std::size_t operator()(const Id& id) const {
        return std::hash<std::vector<bool>>()(id.m_value);
    }
};
}
