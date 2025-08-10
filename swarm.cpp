#include "swarm.h"

#include <random>

void Swarm::addNode(const Id& id, bool isBootstrap) {
    auto node     = std::make_shared<Node>(id, io_context_);
    node->getNode = [this](Id id) { return getNode(id); };
    nodes_.emplace(id, node);
    if (isBootstrap) {
        bootstrapNode_ = node;
    }
}

void Swarm::bootstrapAll() {
    for (auto& [id, node] : nodes_) {
        node->bootstrap(bootstrapNode_->getId());
    }
}

void Swarm::startPeriodicLookups(std::chrono::seconds interval) {
    auto swarm_ptr   = shared_from_this();
    auto found_count = std::make_shared<size_t>(0);
    for (auto& [id, node] : nodes_) {
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> distr(0, g_boot_number - 1);
        Id                              target = distr(gen);
        node->asyncFindNode(target, [&](bool found) {
            if (found) {
                ++(*found_count);
            }
        });
    }

    // Reschedule the next round
    periodic_timer_->expires_after(interval);
    periodic_timer_->async_wait([swarm_ptr, interval, found_count](
                                    const boost::system::error_code& ec) {
        if (!ec)
            LOG("----------------- {} nodes found target ----------------",
                *found_count);
        swarm_ptr->startPeriodicLookups(interval);
    });
};

void Swarm::printStatistics(){

};
