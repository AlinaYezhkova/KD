#include "node.h"

#include "kademlia.h"
#include "utils.h"
#include <bit>

std::random_device Node::random_device;

Node::Node(const Id& id) : id_(id), pool_(id) {
    buckets_.resize(g_id_length + 1);
}

int Node::distance(const Id& id) const {
    return id_ ^ id;
}

Node::Node() {
    generate();
    last_seen_ = std::chrono::system_clock::now();
}

void Node::generate(std::optional<int> seed) {
    /*
     * len=1 (0-9) : 0 - (10^1)-1
     * len=2 (00-99) : 0 - (10^2)-1
     * len=3 (000-999) : 0 - (10^3)-1
     */
    std::uniform_int_distribution<uint64_t> range{0, pow2(g_id_length) - 1};
    std::mt19937                            gen(seed.value_or(random_device()));
    id_ = range(gen);
}

int Node::commonPrefix(const INode& node) const {
    uint64_t dist = distance(node.getId());
    // Remove unused upper bits
    if (g_id_length < 64) {  // TODO - is condition necessary?
        dist &= (~0ULL) >> (64 - g_id_length);
    }
    int leading_zeros = std::countl_zero(dist) - (64 - g_id_length);
    return std::max(0, leading_zeros);
}

bool operator<(const INode& l, const INode& r) {
    return l.getId() < r.getId();
}

bool operator==(const INode& l, const INode& r) {
    return l.getId() == r.getId();
}

bool operator!=(const INode& l, const INode& r) {
    return !(l == r);
}

void Node::bootstrap(std::shared_ptr<INode> node) {
    // node "knows" bootstrap node beforehand
    insert(node);
    kademlia::lookup(*this, *this, *this);
    kademlia::findNode(*this, *this);
}

void Node::remove(std::shared_ptr<INode> node) {
    auto bucket = buckets_[distance(node->getId())];
    bucket.erase(node);
}

void Node::insert(std::shared_ptr<INode> node) {
    int dist = distance(node->getId());
    if (dist > 0) {  // not myself
        auto bucket = buckets_[dist];
        bucket.insert(node);
    }
}

Bucket& Node::getBucket(int bucketNumber) {
    return buckets_[bucketNumber];
}

Pool& Node::getPool() {
    return pool_;
}

const Id& Node::getId() const {
    return id_;
}

const std::chrono::time_point<std::chrono::system_clock> Node::getLastSeen()
    const {
    return last_seen_;
};

void Node::reset() {
    queried_.clear();
    pool_.clear();
}

bool Node::addToQueried(std::shared_ptr<INode> node) {
    queried_.insert(node);
}

bool Node::hasQueried(std::shared_ptr<INode> node) {
    return queried_.find(node) != queried_.end();
}

Bucket::Bucket()
  : set_([](const std::shared_ptr<INode>& a, const std::shared_ptr<INode>& b) {
      return a->getLastSeen() < b->getLastSeen();
  }){};

void Bucket::insert(std::shared_ptr<INode> node) {
    set_.insert(node);
    if (set_.size() > g_bucket_size) {
        auto it = std::prev(set_.end());
        set_.erase(it);
    }
};

void Bucket::erase(std::shared_ptr<INode> node) {
    set_.erase(node);
};
