#pragma once

#include "constants.h"
#include "inode.h"
#include "utils.h"
#include <boost/asio.hpp>
#include <chrono>
#include <unordered_map>
#include <vector>

class Node : public INode {
   private:
    Id                                                          id_;
    boost::asio::io_context&                                    io_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer                                   timer_;
    std::chrono::steady_clock::time_point                       lastSeen_;

    std::unordered_map<int, Bucket> buckets_;  // index by prefix length

   public:
    Node(Id id, boost::asio::io_context& io)
      : id_(id), io_(io), strand_(boost::asio::make_strand(io)), timer_(io) {
        assert(id < pow2(g_id_length));
        lastSeen_ = std::chrono::steady_clock::now();
    }

    const Id&                getId() const override { return id_; }
    boost::asio::io_context& getContext() override { return io_; }
    const boost::asio::strand<boost::asio::io_context::executor_type>&
    getStrand() const override {
        return strand_;
    }
    boost::asio::steady_timer&            getTimer() override { return timer_; }
    std::chrono::steady_clock::time_point getLastSeen() override {
        return lastSeen_;
    }
    std::unordered_map<int, Bucket>& getBuckets() override { return buckets_; }

    int  commonPrefix(Id id) const override;
    void insertNode(Id id) override;
    void asyncInsertNode(Id id) override;
    void bootstrap(const Id& bootstrapId) override;
    void asyncPerformNodeLookup(const Id& target) override;
    void asyncFindNode(const Id& target, std::function<void(bool)> callback) override;
    void sendFindNodeRPC(
        const Id&                            senderId,
        const Id&                            targetId,
        std::function<void(std::vector<Id>)> callback) override;
    void            scheduleRefresh() override;
    void            refreshBuckets() override;
    std::vector<Id> getClosestKnownNodes(const Id& targetId) override;
};
