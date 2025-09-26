#pragma once

#include <cmath>
#include <cstddef>

constexpr double harmonic(int N) {
    double sum = 0.0;
    for (int i = 1; i <= N; ++i) {
        sum += 1.0 / i;
    }
    return sum;
}

static constexpr size_t kIdLength    = 128;
static constexpr size_t kBucketCount = kIdLength;
static constexpr size_t kSwarmSize   = 512;

static constexpr size_t kBucketSize = 8;
static constexpr size_t kAlpha      = 1;
static constexpr size_t kReturn     = 30;
static constexpr size_t IPv4Size    = 4;
static constexpr std::size_t MAX_DGRAM = 1200;  // safe under typical MTU

static constexpr size_t kMsBetweenPeers    = 20;
static constexpr size_t kMsBetweenSearches = kMsBetweenPeers * kSwarmSize *
                                             (1/harmonic(kBucketSize)) *
                                             std::log2(kSwarmSize);
