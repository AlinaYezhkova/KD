#pragma once

#include "utils.h"

static int g_bucket_size = 8;
static int g_pool_size   = 3 * g_bucket_size;
static int g_id_length   = 8;

// max possible swarm size
constexpr uint64_t pow2(int p) {
    return 1 << p;
}
static int g_swarm_size = pow2(g_id_length);

static int g_alpha       = 3;
static int g_boot_number = g_swarm_size;

static int g_lookup_interval  = 10;
static int g_refresh_interval = 5;