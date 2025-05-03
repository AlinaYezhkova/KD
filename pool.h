#pragma once

#include "constants.h"
#include "id.h"
#include "inode.h"
#include "utils.h"
#include <functional>
#include <iostream>
#include <memory>
#include <set>

class Pool
{
private:
    Id m_id;
    using Comparator = std::function<bool(const std::shared_ptr<INode>&, const std::shared_ptr<INode>&)>;
    std::set<std::shared_ptr<INode>, Comparator> m_set;

public:
    Pool() = delete;

    Pool(const Id& id) : m_id(id),
        m_set([id](const std::shared_ptr<INode>& a, const std::shared_ptr<INode>& b) {
            return a->getId().distance(id) < b->getId().distance(id);
                }) {};

    void insert(std::shared_ptr<INode> node)
    {
        m_set.insert(node);
        if(m_set.size() > gPoolSize)
        {
            auto it = std::prev(m_set.end());
            m_set.erase(it);
        }
    };

    bool contains(const std::shared_ptr<INode>& node)
    {
        auto it = m_set.find(node);
        if(it != m_set.end())
        {
            LOG(m_id << " contains fn: " << node->getId())
            LOG("Found target: " << (*(*it)).getId())
        }
        return m_set.find(node) != m_set.end();
    }

    void clear()
    {
        m_set.clear();
    }

    const Id& getId() const
    {
        return m_id;
    }

    bool isEmpty() {return m_set.empty();}
    auto begin() {return m_set.begin();}
    auto end() {return m_set.end();}

    auto begin() const {return m_set.begin();}
    auto end() const {return m_set.end();}

    auto cbegin() const {return m_set.cbegin();}
    auto cend() const {return m_set.cend();}

};
