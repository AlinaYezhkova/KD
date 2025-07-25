#pragma once

#include <fmt/format.h>

#include "constants.h"
#include "fmt/base.h"
// #include "id.h"
#include "inode.h"
#include <functional>
#include <memory>
#include <set>

class Pool {
   private:
    Id id_;
    using Comparator = std::function<bool(const std::shared_ptr<INode>&,
                                          const std::shared_ptr<INode>&)>;
    std::set<std::shared_ptr<INode>, Comparator> set_;

   public:
    // Pool() = delete;
    Pool() {}

    Pool(const Id& id)
      : id_(id)
      , set_([id](const std::shared_ptr<INode>& a,
                  const std::shared_ptr<INode>& b) {
          return a->distance(id) < b->distance(id);
      }){};

    void insert(std::shared_ptr<INode> node) {
        set_.insert(node);
        if (set_.size() > g_pool_size) {
            auto it = std::prev(set_.end());
            set_.erase(it);
        }
    };

    bool contains(const std::shared_ptr<INode>& node) {
        auto it = set_.find(node);
        if (it != set_.end()) {
            fmt::println("Found target {}", (*it)->getId());
        }
        return set_.find(node) != set_.end();
    }

    void clear() { set_.clear(); }

    const Id& getId() const { return id_; }

    bool isEmpty() { return set_.empty(); }
    auto begin() { return set_.begin(); }
    auto end() { return set_.end(); }

    auto begin() const { return set_.begin(); }
    auto end() const { return set_.end(); }

    auto cbegin() const { return set_.cbegin(); }
    auto cend() const { return set_.cend(); }
};
