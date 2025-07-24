#pragma once

#include <cmath>
#include <mutex>

static std::mutex g_log_mutex;

static int g_bucket_size = 4;
static int g_pool_size = 2 * g_bucket_size;
static int g_id_length   = 20;  // 2^20 = 1048576

// max possible swarm size
static int g_swarm_size = pow(2, g_id_length);

static int g_alpha = 2;
static int g_boot_number = g_swarm_size;
