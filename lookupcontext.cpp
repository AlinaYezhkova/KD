#include "lookupcontext.h"

#include "constants.h"

void LookupContext::start() {
    issueNext();
}

void LookupContext::issueNext() {
    if (finished_) {
        return;
    }

    auto                  candidates = find_closest_(target_);
    std::vector<PeerInfo> unqueried;
    for (const auto& pi : candidates)
        if (!queried_.contains(pi.key_)) {
            unqueried.push_back(pi);
        }
    size_t launched = 0;

    for (const PeerInfo& pi : unqueried) {
        queried_.insert(pi.key_);
        ++inflight_;

        send_query_(pi);
        if (++launched >= kAlpha) {
            break;
        }
    }

    maybeFinish();
}

void LookupContext::onResponse(std::vector<PeerInfo> result) {
    if (finished_)
        return;

    // Register response
    --inflight_;

    // Merge new nodes
    for (const auto& pi : result) {
        if (closest_peers_.count(pi.key_) == 0) {
            closest_peers_[pi.key_] = pi;
        }
    }
    issueNext();
}

void LookupContext::maybeFinish() {
    auto start = closest_peers_.begin();
    auto end   = start;
    std::advance(end, kReturn);
    std::map<NodeId, PeerInfo, Comparator> k_closest_peers(start, end);
    if (should_stop_(k_closest_peers) ||
        (inflight_ == 0 && queried_.size() >= kBucketSize)) {
        finished_ = true;
        on_done_(k_closest_peers);
    }
}
