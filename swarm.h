#pragma once

#include "node.h"
#include <unordered_map>

class Swarm
{
private:

    Swarm();
    Swarm(const Swarm&) = delete;
    Swarm& operator=(const Swarm&) = delete;
    std::unordered_map<Id, std::shared_ptr<INode>> value_;

   public:

    static Swarm& getInstance();

    auto begin() { return value_.begin(); }
    auto end() { return value_.end(); }

    auto begin() const { return value_.begin(); }
    auto end() const { return value_.end(); }

    auto cbegin() const { return value_.cbegin(); }
    auto cend() const { return value_.cend(); }

    size_t size() { return value_.size(); }

    std::shared_ptr<INode> getNode(const Id& id) { return value_[id]; };

    friend std::ostream& operator<<(std::ostream& os, const Swarm& swarm);
};
