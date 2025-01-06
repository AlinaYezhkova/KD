#include "node.h"

int Node::distance(const Node& node)
{
    return m_id.distance(node.m_id);
}

bool operator<(const Node& l, const Node& r)
{
    return l.m_id < r.m_id;
}

std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Id: " << node.m_id;
    return os;
}
