#include "id.h"

#include <fmt/base.h>

const std::bitset<kIdLength>& Id::getBits() const {
    return bits_;
}

bool operator<(const Id& a, const Id& b) {
    for (std::size_t i = kIdLength; i-- > 0;) {
        bool abit = a.bits_.test(i);
        bool bbit = b.bits_.test(i);
        if (abit != bbit) {
            return !abit && bbit;  // 0<1
        }
    }
    return false;  // equal
}

bool operator==(const Id& a, const Id& b) {
    return a.bits_ == b.bits_;
}

std::bitset<kIdLength> operator^(const Id& a, const Id& b) {
    return a.bits_ ^ b.bits_;
}

template <> struct fmt::formatter<Id> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Id& id, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", id.getBits().to_string());
    }
};

template <> struct fmt::formatter<std::bitset<kIdLength>> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const std::bitset<kIdLength>& bits, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", bits.to_string());
    }
};