#pragma once

#include "id.h"

class Bucket
{
private:
    std::vector<Id> m_value;

public:
    Bucket() = default;
    Bucket(const Bucket& bucket) : m_value(bucket.m_value) {};

    std::vector<Id>::iterator begin() {
        return m_value.begin();
    }
    std::vector<Id>::iterator end() {
        return m_value.end();
    }

    std::vector<Id>::const_iterator begin() const {
        return m_value.begin();
    }
    std::vector<Id>::const_iterator end() const {
        return m_value.end();
    }

    std::vector<Id>::const_iterator cbegin() const {
        return m_value.cbegin();
    }
    std::vector<Id>::const_iterator cend() const {
        return m_value.cend();
    }

    const Id& at(int n) {
        return m_value[n];
    }

    const size_t size() const {
        return m_value.size();
    }

    const bool empty() const {
        return m_value.empty();
    }

    std::vector<Id> getValue() {
        return m_value;
    }

    bool insert(const Id& id);
    bool remove(const Id& id);
    friend std::ostream& operator<< (std::ostream& os, const Bucket& bucket);
};
