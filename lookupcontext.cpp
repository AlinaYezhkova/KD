#include "lookupcontext.h"

#include "constants.h"
#include "inode.h"
#include "ipeer.h"
#include "messageBuilder.h"

// void LookupContext::findClosest() {
//     node_.find_closest(target_);
// }

// void LookupContext::sendQuery(const PeerInfo& pi) {
//     Message out = MessageBuilder()
//                       .type(MessageType::Find_node_query)
//                       .from(peer_.getPeerInfo())  // current node ID
//                       .to(pi)                     // where it's going
//                       .find(target_)              // who we're looking for
//                       .nonce(nonce_)
//                       .build();
//     peer_.send(out);
// }

// void LookupContext::onDone() {
//     for (const auto& [id, pi] : closest_peers_) {
//         node_.insert(pi);
//     }
//     fmt::println("!!! going to erase '{}' context", nonce_);
//     peer_.endLookup(nonce_);
// }
// bool LookupContext::shouldStop(
//     std::map<NodeId, PeerInfo, Comparator> current_best) {
//     fmt::println("current_best: {} (target: {})", current_best.size(),
//     target_); for (auto& e : current_best) {
//         fmt::println("{} ", e.first);
//     }
//     if (current_best.count(target_) > 0) {
//         fmt::println("yeah! {} found myself {}",
//                      peer_.getPeerInfo().key_,
//                      target_);
//         return true;
//     }
//     return current_best.size() >= kReturn;  // or use convergence logic
// }

// void LookupContext::issueNext() {
//     maybeFinish();
//     if (finished_) {
//         return;
//     }

//     std::vector<PeerInfo> unqueried;
//     for (const auto& [id, pi] : closest_peers_) {
//         if (!queried_.contains(pi.key_)) {
//             unqueried.push_back(pi);
//         }
//     }
//     fmt::println("issueNext: unqueried.size() = {}", unqueried.size());
//     size_t launched = 0;

//     for (const auto& pi : unqueried) {
//         queried_.insert(pi.key_);
//         ++inflight_;

//         Message out = MessageBuilder()
//                           .type(MessageType::Find_node_query)
//                           .from(peer_.getPeerInfo())  // current node ID
//                           .to(pi)                     // where it's going
//                           .find(target_)              // who we're looking
//                           for .nonce(nonce_) .build();
//         peer_.send(out);
//         if (++launched >= kAlpha) {
//             break;
//         }
//     }
// }

void LookupContext::onResponse(std::vector<PeerInfo> result) {
    --inflight_;
    // fmt::println("inflight_ = {}", inflight_);
    if (finished_) {
        return;
    }
    for (const auto& pi : result) {
        closest_peers_[pi.key_] = pi;
    }
    // fmt::println("closest_peers_ = {}", closest_peers_.size());
    maybeFinish();
    if (finished_) {
        return;
    }

    std::vector<PeerInfo> unqueried;
    for (const auto& [id, pi] : closest_peers_) {
        if (!queried_.contains(pi.key_)) {
            unqueried.push_back(pi);
        }
    }
    // fmt::println("issueNext: unqueried.size() = {}", unqueried.size());
    size_t launched = 0;

    for (const auto& pi : unqueried) {
        queried_.insert(pi.key_);
        ++inflight_;

        Message out = MessageBuilder()
                          .type(MessageType::Find_node_query)
                          .from(peer_.getPeerInfo())  // current node ID
                          .to(pi)                     // where it's going
                          .find(target_)              // who we're looking for
                          .nonce(nonce_)
                          .build();
        peer_.send(out);
        if (++launched >= kAlpha) {
            break;
        }
    }
}

void LookupContext::maybeFinish() {
    if (finished_) {
        return;
    }
    auto   start         = closest_peers_.begin();
    auto   end           = start;
    size_t actual_return = std::min(kReturn, closest_peers_.size());
    std::advance(end, actual_return);
    std::map<NodeId, PeerInfo, Comparator> k_closest_peers(comp_);
    k_closest_peers.insert(start, end);

    bool b = false;
    if (k_closest_peers.count(target_) > 0) {
        fmt::println("yeah! {} found myself {}",
                     peer_.getPeerInfo().key_,
                     target_);
        b = true;
    }

    if (b) {
        finished_ = true;
        for (const auto& [id, pi] : closest_peers_) {
            node_.insert(pi);
        }
        // fmt::println("!!! going to erase '{}' context", nonce_);
        peer_.endLookup(nonce_);
    }

    if (inflight_ == 0 && queried_.size() >= kBucketSize) {
        finished_ = true;
        for (const auto& [id, pi] : closest_peers_) {
            node_.insert(pi);
        }
        // fmt::println("!!! going to erase '{}' context", nonce_);
        peer_.endLookup(nonce_);
    }
}
