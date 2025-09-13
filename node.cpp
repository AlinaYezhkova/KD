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

Node::Node(uint64_t id1, uint64_t id2) {
    id_[0] = id1;
    id_[1] = id2;
}

std::vector<PeerInfo> Node::find_K_closest(NodeId target) {
    std::vector<PeerInfo> result;

    for (const auto& e : buckets_) {
        result.insert(result.begin(), e.begin(), e.end());
    }

    std::sort(result.begin(),
              result.end(),
              [&target](const PeerInfo& a, const PeerInfo& b) {
                  auto dist_a = distance(a.key_, target);
                  auto dist_b = distance(b.key_, target);
                  if (dist_a != dist_b) {
                      return dist_a < dist_b;
                  }
                  return (a.last_seen_ > b.last_seen_);
              });

    if (result.size() > kReturn) {
        result.resize(kReturn);
    }
    // fmt::println("find_closest to {} - {} nodes found", target,
    // result.size());
    return result;
};

bool Node::insert(const PeerInfo& pi) {
    int idx = distance(id_, pi.key_);
    // fmt::println("{} inserted {} into bucket {}", id_, pi.key_, idx);

    if (buckets_[idx].size() < kBucketSize) {
        buckets_[idx].insert(pi);
        return true;
    }
    // if (buckets_[idx].size() == kBucketSize) {
    // ping the std::prev(buckets_[idx].end()) as the oldest
    // if it responds -- return;
    // if no response -- delete it; insert const PeerInfo& pi
    // TODO: a list of those who couldn't find a place
    // return false;
    // }
    return false;
}

bool operator<(const PeerInfo& l, const PeerInfo& r) {
    return l.last_seen_ < r.last_seen_;
}
bool operator==(const PeerInfo& l, const PeerInfo& r) {
    return l.last_seen_ == r.last_seen_;
}