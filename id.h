#pragma once

#include "constants.h"
#include <bitset>

class Id {
   private:
    std::bitset<kIdLength> bits_;

   public:
    Id() = default;
    Id(uint64_t id) : bits_(id) {}
    Id(std::bitset<kIdLength> bits) : bits_(std::move(bits)) {}

    const std::bitset<kIdLength>& getBits() const;

    friend bool                   operator<(const Id& a, const Id& b);
    friend bool                   operator==(const Id& a, const Id& b);
    friend std::bitset<kIdLength> operator^(const Id& a, const Id& b);
};