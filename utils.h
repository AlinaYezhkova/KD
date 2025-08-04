#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#ifdef DEBUG
#    define LOG(expr)                                                          \
        {                                                                      \
            std::lock_guard<std::mutex> autolock(g_log_mutex);                 \
            std::ofstream               outfile("log.txt", std::ios::app);     \
            if (!outfile) {                                                    \
                std::cerr << "No log file.\n";                                 \
            } else {                                                           \
                outfile << expr << std::endl;                                  \
                outfile.close();                                               \
            }                                                                  \
        }
#else
#    define LOG(expr)                                                          \
        {}
#endif

constexpr uint64_t pow2(int p) {
    return 1 << p;
}

std::string toBinaryString(uint64_t value);
