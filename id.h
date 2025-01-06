#pragma once

#include "constants.h"
#include <optional>
#include <ostream>
#include <random>
#include <vector>

class Id
{
private:
    static std::random_device m_randomDevice;

    int m_length = gIdLength;
    std::vector<bool> m_value;

    void generate(std::optional<int> seed = std::nullopt);

public:
    Id();
    Id(int length);
    Id(int length, int seed);

    const std::vector<bool>& getValue(){
        return m_value;
    }

    int distance(const Id& id) const;

    friend std::ostream& operator<<(std::ostream& os, const Id& id);
    friend bool operator<(const Id& l, const Id& r);
    friend bool operator==(const Id& l, const Id& r);

};

