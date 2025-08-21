#include "node.h"

#include "utils.h"
#include <boost/functional/hash.hpp>

Node::Node(const std::string& address_port) {
    boost::hash<std::string> hasher;
    size_t hash1 = hasher(address_port + "0");  // salt to diversify
    size_t hash2 = hasher(address_port + "1");

    id_[0] = static_cast<uint64_t>(hash1);
    id_[1] = static_cast<uint64_t>(hash2);
}

std::vector<PeerInfo> Node::find_closest(NodeId target) {
    std::vector<PeerInfo> result;

    for (const auto& e : buckets_) {
        result.insert(result.begin(), e.begin(), e.end());
    }

    std::sort(result.begin(),
              result.end(),
              [&target](const PeerInfo& a, const PeerInfo& b) {
                  return (xor_id(a.key_, target) < xor_id(b.key_, target));
              });

    if (result.size() > kReturn) {
        result.resize(kReturn);
    }
    // fmt::println("[{}] - getClosestKnownNodes to {} - {} nodes found",
    //              id_, targetId,
    //              all_known_ids.size());
    return result;
};

void Node::insert(const PeerInfo& pi) {
    if (pi.key_ == id_) {
        // don't include self
        return;
    }
    int idx = distance(id_, pi.key_);
    if (buckets_[idx].size() < kBucketSize) {
        buckets_[idx].insert(pi);
        return;
    }
    if (buckets_[idx].size() == kBucketSize) {
        // ping the std::prev(buckets_[idx].end()) as the oldest
        // if it responds -- return;
        // if no response -- delete it; insert const PeerInfo& pi
        // TODO: a list of those who couldn't find a place
        return;
    }
}

bool operator<(const PeerInfo& l, const PeerInfo& r) {
    return l.last_seen_ < r.last_seen_;
}
bool operator==(const PeerInfo& l, const PeerInfo& r) {
    return l.last_seen_ == r.last_seen_;
}