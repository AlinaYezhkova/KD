#pragma once

#include <fmt/core.h>
#include <fstream>
#include <mutex>
#include <string_view>
#include <iostream>

static const char* g_file_path = "/home/odal/QtProjects/KD/log.txt";

inline std::mutex g_log_mutex;

inline void log_impl(fmt::string_view fmt_str, fmt::format_args args) {
    std::lock_guard<std::mutex> lock(g_log_mutex);
    static std::ofstream out(g_file_path, std::ios::app);
    if (!out) {
        std::cerr << "No log file.\n";
        return;
    }
    out << fmt::vformat(fmt_str, args) << '\n';
    out.flush();  // make visible immediately
}

template <typename... Args>
inline void log_fmt(fmt::format_string<Args...> fmt_str, Args&&... args) {
    log_impl(fmt_str, fmt::make_format_args(args...));
}

#define LOG(...) log_fmt(__VA_ARGS__)

std::string toBinaryString(uint64_t value);