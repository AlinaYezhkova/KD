#pragma once

#include "node.h"
#include <boost/asio.hpp>
#include <unordered_map>

class Swarm : public std::enable_shared_from_this<Swarm> {
   private:
    boost::asio::io_context&                      io_context_;
    std::unordered_map<Id, std::shared_ptr<Node>> nodes_;
    std::shared_ptr<Node>                         bootstrapNode_;
    std::shared_ptr<boost::asio::steady_timer>    periodic_timer_;

   public:
    Swarm(boost::asio::io_context& io) : io_context_(io) {
        periodic_timer_ = std::make_shared<boost::asio::steady_timer>(io);
    }

    void                  addNode(const Id& id, bool isBootstrap = false);
    void                  bootstrapAll();
    void                  startPeriodicLookups(std::chrono::seconds interval);
    std::shared_ptr<Node> getNode(const Id& id) {
        auto it = nodes_.find(id);
        return it != nodes_.end() ? it->second : nullptr;
    }
    const std::unordered_map<Id, std::shared_ptr<Node>>& nodes() const {
        return nodes_;
    }
};
