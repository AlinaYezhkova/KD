#include "utils.h"

std::string toBinaryString(uint64_t value) {
    std::string result;
    result.reserve(g_id_length);

    for (int i = g_id_length - 1; i >= 0; --i) {
        result += (value & (1ULL << i)) ? '1' : '0';
    }

    return result;
}
