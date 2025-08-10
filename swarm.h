#pragma once

#include "node.h"
#include <boost/asio.hpp>
#include <random>
#include <unordered_map>

class Swarm : public std::enable_shared_from_this<Swarm> {
   private:
    boost::asio::io_context&                      io_context_;
    std::unordered_map<Id, std::shared_ptr<Node>> nodes_;
    std::shared_ptr<Node>                         bootstrapNode_;
    std::shared_ptr<boost::asio::steady_timer>    periodic_timer_;

   public:
    // std::random_device              rd_;
    std::mt19937                    gen_;
    std::uniform_int_distribution<> distr_;

    Swarm(boost::asio::io_context& io, uint32_t seed)
      : io_context_(io), gen_(seed), distr_(0, g_boot_number - 1) {
        periodic_timer_ = std::make_shared<boost::asio::steady_timer>(io);
    }

    void                  addNode(const Id& id, bool isBootstrap = false);
    void                  bootstrapAll();
    void                  startPeriodicLookups(std::chrono::seconds interval);
    void                  printStatistics();
    std::shared_ptr<Node> getNode(const Id& id) {
        auto it = nodes_.find(id);
        return it != nodes_.end() ? it->second : nullptr;
    }
    const std::unordered_map<Id, std::shared_ptr<Node>>& nodes() const {
        return nodes_;
    }
};
