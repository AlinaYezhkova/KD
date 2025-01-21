#pragma once

#include "node.h"
#include <map>

class Swarm
{
private:

    Swarm();
    Swarm(const Swarm&) = delete;
    Swarm& operator=(const Swarm&) = delete;
    std::map<Id, std::shared_ptr<INode>> m_value;

public:

    static Swarm& getInstance();

    std::map<Id, std::shared_ptr<INode>>::iterator begin() {
        return m_value.begin();
    }
    std::map<Id, std::shared_ptr<INode>>::iterator end() {
        return m_value.end();
    }

    std::map<Id, std::shared_ptr<INode>>::const_iterator begin() const {
        return m_value.begin();
    }
    std::map<Id, std::shared_ptr<INode>>::const_iterator end() const {
        return m_value.end();
    }

    std::map<Id, std::shared_ptr<INode>>::const_iterator cbegin() const {
        return m_value.cbegin();
    }
    std::map<Id, std::shared_ptr<INode>>::const_iterator cend() const {
        return m_value.cend();
    }

    size_t size() {
        return m_value.size();
    }

    std::shared_ptr<INode> getNode(const Id& id);

    friend std::ostream& operator<<(std::ostream& os, const Swarm& swarm);
};
