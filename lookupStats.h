#pragma once
#include <atomic>

struct LookupStats {
    std::atomic<std::uint64_t> total_peers_seen{0};
    std::atomic<std::uint64_t> hop_count{0};
    std::atomic<std::uint64_t> total_hop_count{0};

    void addFoundNode() noexcept {
        total_peers_seen.fetch_add(1, std::memory_order_relaxed);
    }
    std::uint64_t getFoundNodes() const noexcept {
        return total_peers_seen.load(std::memory_order_relaxed);
    }
    void resetFoundNodes() noexcept {
        total_peers_seen.store(0, std::memory_order_relaxed);
    }
    //------------------------------------------------------------------------
    void addHopCount() noexcept {
        hop_count.fetch_add(1, std::memory_order_relaxed);
    }
    void mergeHopCounts() noexcept {
        total_hop_count.fetch_add(hop_count.load(std::memory_order_relaxed),
                                  std::memory_order_relaxed);
        hop_count.store(0, std::memory_order_relaxed);
    }
    std::uint64_t getHopCount() const noexcept {
        return hop_count.load(std::memory_order_relaxed);
    }
    std::uint64_t getTotalHopCounts() const noexcept {
        return total_hop_count.load(std::memory_order_relaxed);
    }
    void resetHopCount() noexcept {
        hop_count.store(0, std::memory_order_relaxed);
    }
    void resetTotalHopCounts() noexcept {
        total_hop_count.store(0, std::memory_order_relaxed);
    }
};
