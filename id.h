#pragma once

#include "constants.h"
#include "fmt/base.h"
#include "utils.h"
#include <cassert>
#include <optional>
#include <random>

class Id {
   public:
    int      length_ = g_id_length;
    uint64_t value_;
    void     generate(std::optional<int> seed = std::nullopt);

   private:
    static std::random_device random_device;

   public:
    Id();
    Id(uint64_t val) : value_(val) { assert(val < pow2(g_id_length)); }
    Id(const Id& id) : value_(id.value_){};
    // Id(int seed);

    uint64_t commonPrefix(const Id& id) const;
    uint64_t distance(const Id& id) const;

    friend bool operator<(const Id& l, const Id& r);
    friend bool operator==(const Id& l, const Id& r);
    friend bool operator!=(const Id& l, const Id& r);
};

template <> struct std::hash<Id> {
    std::size_t operator()(const Id& id) const {
        return std::hash<uint64_t>()(id.value_);
    }
};

template <> struct fmt::formatter<Id> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Id& id, FormatContext& ctx) const {
        std::string result = std::to_string(id.value_) + '\n';
        return fmt::format_to(ctx.out(), "{}", result);
    }
};
