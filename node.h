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
    // Node(const std::string& address_port);
    // Node(uint64_t id1, uint64_t id2);

    std::vector<PeerInfo> find_K_closest(Id target) override;
    bool                  insert(const PeerInfo& pi) override;
    const Id&             get_id() const override { return id_; }
};
