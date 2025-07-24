#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
// #define LOG(expr) { \
//     std::cout << expr << std::endl; \
// };

#define LOG(expr) {}

#ifdef DEBUG
#define LOG(expr) \
    { \
        std::lock_guard<std::mutex> autolock(g_log_mutex); \
        std::ofstream outfile("log.txt", std::ios::app); \
        if (!outfile) { \
            std::cerr << "No log file.\n"; \
        } else { \
            outfile << expr << std::endl; \
            outfile.close(); \
        } \
    }
#endif

constexpr uint64_t pow2(int p) {
    return 1 << p;
}
