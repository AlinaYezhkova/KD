#pragma once

#include <cmath>
#include <mutex>

static std::mutex gLogMutex;

static int gBucketSize = 4;
static int gPoolSize = 2 * gBucketSize;
static int gIdLength = 8;

// max possible swarm size
static int gSwarmSize = pow(2, gIdLength);

static int gAlpha = 2;
static int gBootNumber = gSwarmSize;
