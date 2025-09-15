#pragma once
#include <atomic>

struct LookupStats {
    std::atomic<std::uint64_t> total_peers_seen{0};
    std::atomic<std::uint64_t> hop_count{0};

    void addFoundNode() noexcept {
        total_peers_seen.fetch_add(1, std::memory_order_relaxed);
    }
    std::uint64_t getFoundNodes() const noexcept {
        return total_peers_seen.load(std::memory_order_relaxed);
    }

    void addHopCount() noexcept {
        hop_count.fetch_add(1, std::memory_order_relaxed);
    }
    std::uint64_t getHopCount() const noexcept {
        return hop_count.load(std::memory_order_relaxed);
    }

    void reset() noexcept {
        total_peers_seen.store(0, std::memory_order_relaxed);
    }
};
