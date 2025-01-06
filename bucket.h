#pragma once

#include "id.h"

class Bucket
{
private:
    std::vector<Id> m_ids;
    int m_size;

public:
    Bucket();
};
