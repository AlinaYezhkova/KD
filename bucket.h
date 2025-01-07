#pragma once

#include "id.h"

class Bucket
{
private:
    std::vector<Id> m_value;

public:
    Bucket();

    bool insert(const Id& id);
    bool remove(const Id& id);
};
