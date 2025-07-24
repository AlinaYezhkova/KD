#pragma once

#include "id.h"

class Bucket
{
private:
 std::vector<Id> value_;

public:
    Bucket() = default;
    Bucket(const Bucket& bucket) : value_(bucket.value_){};

    std::vector<Id>::iterator begin() { return value_.begin(); }
    std::vector<Id>::iterator end() { return value_.end(); }

    std::vector<Id>::const_iterator begin() const { return value_.begin(); }
    std::vector<Id>::const_iterator end() const { return value_.end(); }

    std::vector<Id>::const_iterator cbegin() const { return value_.cbegin(); }
    std::vector<Id>::const_iterator cend() const { return value_.cend(); }

    const Id& at(int n) { return value_[n]; }

    const size_t size() const { return value_.size(); }

    const bool empty() const { return value_.empty(); }

    std::vector<Id> getValue() { return value_; }

    void insert(const Id& id);
    void remove(const Id& id);
    friend std::ostream& operator<< (std::ostream& os, const Bucket& bucket);
};
