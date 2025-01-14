#pragma once

#include "id.h"

class Bucket
{
private:
    std::vector<Id> m_value;

public:
    Bucket() = default;

    Id& at(int n) {
        return m_value[n];
    }

    const size_t size() {
        return m_value.size();
    }

    bool insert(const Id& id);
    bool remove(const Id& id);
    std::vector<Id> copyTo(std::vector<Id>& result);
};
