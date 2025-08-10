#include "node.h"

#include "lookupcontext.h"
#include "utils.h"
#include <random>

void Node::insertNode(Id id) {
    int bucket_index = g_id_length - commonPrefix(id);
    buckets_[bucket_index].insert(id);
    buckets_[bucket_index].setGetNodeCallback([this](const Id& id) {
        return getNode(id);  // from swarm or lambda
    });

    // LOG("[{}] inserted {} into bucket {}", id_, id, bucket_index);
}

void Node::asyncInsertNode(Id id) {
    // LOG("Posting asyncInsertNode: insert {} into {}" , id, id_);
    boost::asio::post(strand_, [self = shared_from_this(), id] {
        self->insertNode(
            id);  // serialized even if called from different threads
    });
}

void Node::bootstrap(const Id& bootstrapId) {
    if (bootstrapId == id_)
        return;

    auto bootstrap_node = getNode(bootstrapId);
    if (!bootstrap_node)
        return;

    insertNode(bootstrap_node->getId());
    asyncPerformNodeLookup(id_);
}

void Node::asyncPerformNodeLookup(const Id& target) {
    auto ctx = std::make_shared<LookupContext>(*this, target);
    ctx->start();
}

void Node::asyncFindNode(const Id& target, std::function<void(bool)> callback) {
    auto ctx = std::make_shared<LookupContext>(*this, target);
    ctx->setStopWhenFound(true);
    ctx->setCompletionCallback(std::move(callback));
    ctx->start();
}

void Node::sendFindNodeRPC(const Id&                            interId,
                           const Id&                            targetId,
                           std::function<void(std::vector<Id>)> callback) {
    boost::asio::post(
        strand_,
        [self = shared_from_this(),
         interId,
         targetId,
         cb = std::move(callback)] {
            // Simulate async delay
            self->getTimer().expires_after(std::chrono::milliseconds(2));
            self->getTimer().async_wait(
                [self, interId, targetId, cb](
                    const boost::system::error_code& ec) {
                    if (ec)
                        return;  // Timer was cancelled

                    // This is where the logic of FIND_NODE happens.
                    std::vector<Id> result =
                        self->getNode(interId)->getClosestKnownNodes(targetId);

                    // Optionally log
                    // fmt::println("[{}] Responding to FIND_NODE({}, {}
                    // results)",
                    //              toBinaryString(self->getId()),
                    //              toBinaryString(targetId),
                    //              result.size());

                    cb(std::move(result));
                });
        });
}

void Node::scheduleRefresh() {
    boost::asio::post(strand_, [self = shared_from_this()] {
        self->getTimer().expires_after(
            std::chrono::seconds(g_refresh_interval));
        self->getTimer().async_wait(boost::asio::bind_executor(
            self->getStrand(),
            [self](const boost::system::error_code& ec) {
                if (!ec)
                    self->refreshBuckets();
            }));
    });
}

void Node::refreshBuckets() {
    // fmt::println("[{}] Refreshing all buckets", toBinaryString(id_));
    boost::asio::post(strand_, [self = shared_from_this()] {
        for (auto& [index, bucket] : self->getBuckets()) {
            if (!bucket.empty()) {
                // Refresh by looking up a random ID in the bucket's range
                Id random_id = bucket.randomIdInRange();
                self->asyncPerformNodeLookup(random_id);
            }
        }
    });
}

std::vector<Id> Node::getClosestKnownNodes(
    const Id& targetId) {  // g_pool_size or fewer
    std::set<Id> all;      // using set to auto-filter duplicates

    for (const auto& [index, bucket] : buckets_) {
        all.insert(bucket.begin(), bucket.end());
    }
    // all.erase(id_);  // don’t include self

    std::vector<Id> all_known_ids(all.begin(), all.end());

    std::sort(all_known_ids.begin(),
              all_known_ids.end(),
              [&targetId](const Id& a, const Id& b) {
                  return (a ^ targetId) < (b ^ targetId);
              });

    if (all_known_ids.size() > g_pool_size)
        all_known_ids.resize(g_pool_size);
    // fmt::println("[{}] - getClosestKnownNodes to {} - {} nodes found",
    //              id_, targetId,
    //              all_known_ids.size());
    // fmt::println("{} : getClosestKnownNodes", id_);
    // for(auto& e: all_known_ids){
    //     fmt::print("{}, ", e);
    // }
    // fmt::println("");
    return all_known_ids;
}

int Node::commonPrefix(Id id) const {
    uint64_t dist = id_ ^ id;
    // Remove unused upper bits
    if (g_id_length < 64) {  // TODO - is condition necessary?
        dist &= (~0ULL) >> (64 - g_id_length);
    }
    int leading_zeros = std::countl_zero(dist) - (64 - g_id_length);
    return std::max(0, leading_zeros);
}

void Node::print() {
    fmt::println("----{} ({})----", id_, toBinaryString(id_));
    for (auto& [dist, bucket] : buckets_) {
        fmt::println("{}:", dist);
        for (auto& e : bucket) {
            fmt::print("{}, ", e);
        }
        fmt::println("");
    }
}

void Bucket::setGetNodeCallback(
    std::function<std::shared_ptr<INode>(const Id&)> cb) {
    getNode = std::move(cb);
}

void Bucket::insert(const Id& id) {
    // auto it = std::find(ids_.begin(), ids_.end(), id);
    // if (it != ids_.end()) {
    //     // Already known, refresh
    //     ids_.erase(it);
    //     ids_.push_back(id);
    //     return;
    // }

    if (ids_.size() < g_bucket_size) {
        ids_.push_back(id);
        return;
    }
    // Eviction logic based on lastSeen
    // if (!getNode)
    //     return;  // Fail-safe

    // auto oldest_it   = ids_.begin();
    // auto oldest_node = getNode(*oldest_it);
    // for (auto current = ids_.begin(); current != ids_.end(); ++current) {
    //     auto node = getNode(*current);
    //     if (node && oldest_node &&
    //         node->getLastSeen() < oldest_node->getLastSeen()) {
    //         oldest_it   = current;
    //         oldest_node = node;
    //     }
    // }

    // ids_.erase(oldest_it);
    // ids_.push_back(id);
}

Id Bucket::randomIdInRange() {
    std::random_device              rd;  // obtain a random number from hardware
    std::mt19937                    gen(rd());  // seed the generator
    std::uniform_int_distribution<> distr(
        0,
        g_bucket_size - 1);  // define the range

    return ids_[distr(gen)];
}
