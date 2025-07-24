#pragma once

#include "bucket.h"
#include "id.h"
#include "inode.h"
#include "packetcounter.h"
#include "pool.h"

class Node : public INode {
   private:
    Id                                id_;
    std::vector<Bucket>               buckets_;
    std::set<std::shared_ptr<INode> > queried_;
    Pool                              pool_;
    PacketCounter                     packet_counter_;

   public:
    Node() = delete;
    // Node(const Node& node)
    //   : id_(node.id_)
    //   , buckets_(node.buckets_)
    //   , pool_(node.pool_)
    //   , packet_counter_(node.packet_counter_){}
    Node(const Id& id);

    int       distance(const INode& node) override;
    void      bootstrap(Id& bootstrapId) override;
    void      remove(const Id& id) override;
    void      insert(const Id& id) override;
    Bucket&   getBucket(int bucketNumber) override;
    Pool&     getPool() override;
    const Id& getId() const override;
    void      reset() override;
    bool      addToQueried(std::shared_ptr<INode> node) override;
    bool      hasQueried(std::shared_ptr<INode> node) override;

    bool operator<(const INode& r) const override;
    bool operator==(const INode& r) const override;
    bool operator!=(const INode& r) const override;
};
