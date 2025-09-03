#include "lookupcontext.h"

#include "constants.h"
#include "inode.h"
#include "ipeer.h"
#include "messageBuilder.h"

// void LookupContext::findClosest() {
//     node_.find_closest(target_);
// }

void LookupContext::sendQuery(const PeerInfo& pi) {
    Message out = MessageBuilder()
                      .type(MessageType::Find_node_query)
                      .from(peer_.getPeerInfo())  // current node ID
                      .to(pi)                     // where it's going
                      .find(target_)              // who we're looking for
                      .nonce(nonce_)
                      .build();
    peer_.send(out);
}

void LookupContext::onDone() {
    finished_ = true;
    for (const auto& [id, pi] : ordered_peers_) {
        node_.insert(pi);
    }
    // fmt::println("!!! going to erase '{}' context", nonce_);
    peer_.endLookup(nonce_);
}

bool LookupContext::shouldStop(
    std::map<NodeId, PeerInfo, Comparator> current_best) {
    // fmt::println("current_best: {} (target: {})", current_best.size(),
    // target_); for (auto& e : current_best) {
    //     fmt::println("{} ", e.first);
    // }
    if (current_best.count(target_) > 0) {
        fmt::println("yeah");
        return true;
    }
    return current_best.size() >= kReturn;  // or use convergence logic
}

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
        ordered_peers_[pi.key_] = pi;
    }
    // fmt::println("closest_peers_ = {}", closest_peers_.size());
    maybeFinish();
    if (finished_) {
        return;
    }

    std::vector<PeerInfo> unqueried;
    for (const auto& [id, pi] : ordered_peers_) {
        if (!queried_.contains(pi.key_)) {
            unqueried.push_back(pi);
        }
    }
    // fmt::println("issueNext: unqueried.size() = {}", unqueried.size());
    size_t launched = 0;

    for (const auto& pi : unqueried) {
        queried_.insert(pi.key_);
        ++inflight_;
        sendQuery(pi);
        if (++launched >= kAlpha) {
            break;
        }
    }
}

void LookupContext::maybeFinish() {
    if (finished_) {
        return;
    }
    auto   start        = ordered_peers_.begin();
    size_t closest_size = std::min(kReturn, ordered_peers_.size());
    auto   closest_end  = start;
    std::advance(closest_end, closest_size);

    std::map<NodeId, PeerInfo, Comparator> closest_peers(comp_);
    closest_peers.insert(start, closest_end);

    if (shouldStop(closest_peers)) {
        onDone();
    }
    if (inflight_ == 0 && queried_.size() >= kReturn) {
        onDone();
    }
}
