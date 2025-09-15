#pragma once

#include <cmath>
#include <cstddef>

static constexpr size_t kIdLength    = 8;
static constexpr size_t kBucketCount = kIdLength;
static constexpr size_t kSwarmSize   = pow(2, kIdLength) - 1;

static constexpr size_t kBucketSize = 8;
static constexpr size_t kAlpha      = 1;
static constexpr size_t kReturn     = 20;
static constexpr size_t IPv4Size    = 4;