#pragma once

#include <fstream>
#include <sstream>

// #define LOG(expr) { \
//     std::cout << expr << std::endl; \
// };

#define LOG(expr) {}

#ifdef DEBUG
#define LOG(expr) { \
    std::lock_guard<std::mutex> autolock( gLogMutex ); \
    std::ofstream outfile("log.txt", std::ios::app); \
    if (!outfile) { \
        std::cerr << "No log file.\n"; \
    } else { \
        outfile << expr << std::endl; \
        outfile.close(); \
    } \
}
#endif
