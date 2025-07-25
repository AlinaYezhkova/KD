#pragma once

#include "inode.h"
#include "packetcounter.h"
#include "pool.h"
#include <random>

class Node : public INode {
   public:
    Id                                id_;
    std::vector<Bucket>               buckets_;
    std::set<std::shared_ptr<INode> > queried_;
    Pool                              pool_;
    PacketCounter                     packet_counter_;

   private:
    static std::random_device random_device;
    void                      generate(std::optional<int> seed = std::nullopt);
    uint64_t                  commonPrefix(const INode& node) const;

   public:
    Node();
    // Node(const Node& node)
    //   : id_(node.id_)
    //   , buckets_(node.buckets_)
    //   , pool_(node.pool_)
    //   , packet_counter_(node.packet_counter_){}
    Node(const Id& id);

    int       distance(const Id& id) const override;
    void      bootstrap(Id& bootstrapId) override;
    void      remove(const Id& id) override;
    void      insert(const Id& id) override;
    Bucket&   getBucket(int bucketNumber) override;
    Pool&     getPool() override;
    const Id& getId() const override;
    void      reset() override;
    bool      addToQueried(std::shared_ptr<INode> node) override;
    bool      hasQueried(std::shared_ptr<INode> node) override;
};
