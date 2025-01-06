#pragma once

#include <optional>
#include <ostream>
#include <random>
#include <vector>

class Id
{
private:
    std::random_device m_randomDevice;
    int m_seed = 0;
    int m_length = 0;
    std::vector<bool> m_value;

    void generate(std::optional<int> seed = std::nullopt);

public:
    Id();
    Id(int length);
    Id(int length, int seed);

    int distance(const Id& id);

    friend std::ostream& operator<<(std::ostream& os, const Id& id);
    static void testId();
};

