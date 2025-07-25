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

uint64_t Node::commonPrefix(const INode& node) const {
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

void Node::bootstrap(Id& bootstrapId) {
    // node "knows" bootstrap node beforehand
    insert(bootstrapId);
    kademlia::lookup(*this, *this, *this);
    kademlia::findNode(*this, *this);
}

void Node::remove(const Id& id) {
    auto bucket = buckets_[distance(id)];
    bucket.erase(std::find(bucket.begin(), bucket.end(), id));
}

void Node::insert(const Id& id) {
    int dist = distance(id);
    if (dist > 0) {  // not myself
        auto bucket = buckets_[dist];
        if (std::find(bucket.begin(), bucket.end(), id) != bucket.end() ||
            bucket.size() == g_bucket_size) {
            return;
        }
        bucket.push_back(id);
        // TODO: update last seen
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
