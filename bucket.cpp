#include "bucket.h"

#include <fmt/core.h>

#include <algorithm>

void Bucket::insert(const Id& id) {
    if (std::find(value_.begin(), value_.end(), id) != value_.end() ||
        value_.size() == g_bucket_size) {
        return;
    }
    value_.push_back(id);
    // TODO: update last seen
}

void Bucket::remove(const Id& id) {
    value_.erase(std::find(value_.begin(), value_.end(), id));
}

template <> struct fmt::formatter<Bucket> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Bucket& b, FormatContext& ctx) const {
        std::string result;
        for (auto it = b.begin(); it != b.end(); ++it) {
            result = fmt::format("{}, {}", result, it->value_);
        }
        return fmt::format_to(ctx.out(), "{}", result);
    }
};
