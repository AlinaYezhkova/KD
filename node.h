#pragma once

#include "constants.h"
#include "inode.h"
#include <array>
#include <set>

class Node : public INode {
   private:
    Id                                           id_;
    std::array<std::set<PeerInfo>, kBucketCount> buckets_;

   public:
    Node() = delete;
    Node(uint64_t id) : id_(id) {}


    std::vector<PeerInfo> find_K_closest(Id target) override;
    bool                  insert(const PeerInfo& pi) override;
    const Id&             get_id() const override { return id_; }
};
