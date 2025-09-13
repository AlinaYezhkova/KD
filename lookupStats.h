#pragma once
#include <atomic>

struct LookupStats {
    std::atomic<std::uint64_t> total_peers_seen{0};

    void addFoundNode() noexcept {
        total_peers_seen.fetch_add(1, std::memory_order_relaxed);
    }

    void reset() noexcept {
        total_peers_seen.store(0, std::memory_order_relaxed);
    }

    std::uint64_t get() const noexcept {
        return total_peers_seen.load(std::memory_order_relaxed);
    }
};
