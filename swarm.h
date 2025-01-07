#pragma once

#include "node.h"
#include <vector>

class Swarm
{
private:
    std::vector<Node> m_value;


public:
    Swarm();

    const std::vector<Node>& getValue() {
        return m_value;
    }

    void initiate();
    void bootstrap();

    friend std::ostream& operator<<(std::ostream& os, const Swarm& swarm);

};
