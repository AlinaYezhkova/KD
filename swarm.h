#pragma once

#include "node.h"

class Swarm
{
private:

    Swarm();
    Swarm(const Swarm&) = delete;
    Swarm& operator=(const Swarm&) = delete;

    // TODO: iterators


    std::map<Id, Node> m_value;

public:

    static Swarm& getInstance();

    Node& getNode(const Id& id);
    inline std::map<Id, Node>& getValue() {
        return m_value;
    }

    void bootstrap();

    friend std::ostream& operator<<(std::ostream& os, const Swarm& swarm);

};
