#include "id.h"

#include "utils.h"
#include <bit>
std::random_device Id::random_device;

Id::Id() {
    generate();
}

void Id::generate(std::optional<int> seed) {
    /*
     * len=1 (0-9) : 0 - (10^1)-1
     * len=2 (00-99) : 0 - (10^2)-1
     * len=3 (000-999) : 0 - (10^3)-1
     */
    std::uniform_int_distribution<uint64_t> range{0, pow2(g_id_length) - 1};
    std::mt19937                            gen(seed.value_or(random_device()));
    value_ = range(gen);
}

uint64_t Id::commonPrefix(const Id& id) const {
    uint64_t dist = distance(id);
    // Remove unused upper bits
    if (length_ < 64) {  // TODO - is condition necessary?
        dist &= (~0ULL) >> (64 - length_);
    }
    int leading_zeros = std::countl_zero(dist) - (64 - length_);
    return std::max(0, leading_zeros);
}

uint64_t Id::distance(const Id& id) const {
    return value_ ^ id.value_;
}

bool operator<(const Id& l, const Id& r) {
    return l.value_ < r.value_;
}

bool operator==(const Id& l, const Id& r) {
    return l.value_ == r.value_;
}

bool operator!=(const Id& l, const Id& r) {
    return !(l == r);
}
