#pragma once

#include <cmath>

static int gBucketSize = 4;
static int gFindNodeSize = 2 * gBucketSize;
static int gIdLength = 8;

// max possible swarm size
static int gSwarmSize = pow(2, gIdLength);

static int gSpreadNumber = 3;
