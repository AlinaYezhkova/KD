#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
// #define LOG(expr) { \
//     std::cout << expr << std::endl; \
// };

#define LOG(expr) {}

// #ifdef DEBUG
// #define LOG(expr) \
//     { \
//         std::lock_guard<std::mutex> autolock(g_log_mutex); \
//         std::ofstream outfile("log.txt", std::ios::app); \
//         if (!outfile) { \
//             std::cerr << "No log file.\n"; \
//         } else { \
//             outfile << expr << std::endl; \
//             outfile.close(); \
//         } \
//     }
// #endif

// template <typename T, std::size_t N, typename Compare = std::less<T>>
// class FixedSizeOrderedSet {
//    public:
//     using value_type     = T;
//     using container_type = std::set<T, Compare>;
//     using iterator       = typename container_type::iterator;

//     bool insert(const T& value) {
//         auto [it, inserted] = data_.insert(value);
//         if (!inserted)
//             return false;
//         if (data_.size() > N) {
//             auto last_it = std::prev(data_.end());
//             data_.erase(last_it);
//         }

//         return true;
//     }

//     const std::set<T, Compare>& data() const { return data_; }

//     std::size_t size() const { return data_.size(); }

//     void clear() { data_.clear(); }

//     bool contains(const T& value) const {
//         return data_.find(value) != data_.end();
//     }

//    private:
//     std::set<T, Compare> data_;
// };

constexpr uint64_t pow2(int p) {
    return 1 << p;
}

std::string toBinaryString(uint64_t value);
