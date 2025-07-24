#include "node.h"

#include "kademlia.h"
#include "swarm.h"
#include "utils.h"

Node::Node(const Id& id) : id_(id), pool_(id) {
    buckets_.resize(g_id_length + 1);
}

int Node::distance(const INode& node) {
    return id_.distance(node.getId());
}

void Node::bootstrap(Id& bootstrapId) {
    // node "knows" bootstrap node beforehand
    insert(bootstrapId);
    kademlia::lookup(*this, *this, *this);
    kademlia::findNode(*this, *this);
}

void Node::remove(const Id& id) {
    buckets_[id_.distance(id)].remove(id);
}

void Node::insert(const Id& id) {
    int dist = id_.distance(id);
    if (dist > 0) {
        buckets_[dist].insert(id);
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

bool Node::operator<(const INode& r) const {
    return id_ < r.getId();
}

bool Node::operator==(const INode& r) const {
    return id_ == r.getId();
}

bool Node::operator!=(const INode& r) const {
    return id_ != r.getId();
}
