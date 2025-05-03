#pragma once

#include "node.h"
#include <unordered_map>

class Swarm
{
private:

    Swarm();
    Swarm(const Swarm&) = delete;
    Swarm& operator=(const Swarm&) = delete;
    std::unordered_map<Id, std::shared_ptr<INode>> m_value;

public:

    static Swarm& getInstance();

    auto begin() { return m_value.begin(); }
    auto end() { return m_value.end(); }

    auto begin() const { return m_value.begin(); }
    auto end() const { return m_value.end(); }

    auto cbegin() const { return m_value.cbegin(); }
    auto cend() const { return m_value.cend(); }

    size_t size() { return m_value.size(); }

    std::shared_ptr<INode> getNode(const Id& id) { return m_value[id]; };

    friend std::ostream& operator<<(std::ostream& os, const Swarm& swarm);
};
