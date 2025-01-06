#pragma once

#include "node.h"
#include <vector>

class Swarm
{
private:
    std::vector<Node> m_value;

public:
    Swarm();

    void initiate();

    const std::vector<Node>& getValue() {
        return m_value;
    }

    friend std::ostream& operator<<(std::ostream& os, const Swarm& swarm);

};
