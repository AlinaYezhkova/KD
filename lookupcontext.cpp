#include "lookupcontext.h"

#include "constants.h"
#include "inode.h"
#include "ipeer.h"
#include "messageBuilder.h"

void LookupContext::start() {
    auto closest = node_.find_closest(target_);
    for (const auto& pi : closest) {
        closest_peers_[pi.key_] = pi;
    }
    issueNext();
}

void LookupContext::issueNext() {
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
        sendFindNodeQuery(pi);
        if (++launched == kAlpha) {
            break;
        }
    }
}

void LookupContext::sendFindNodeQuery(const PeerInfo& pi) {
    Message query = MessageBuilder()
                        .type(MessageType::Find_node_query)
                        .from(peer_.getPeerInfo())
                        .to(pi)
                        .find(target_)
                        .nonce(nonce_)
                        .build();
    peer_.send(query);
}

void LookupContext::onResponse(std::vector<PeerInfo> peers_found) {
    --inflight_;
    // if redundant message:
    if (finished_) {
        return;
    }
    
    for (const auto& pi : peers_found) {
        closest_peers_[pi.key_] = pi;
    }
    // fmt::println("closest_peers_ = {}", closest_peers_.size());
    issueNext();
}

void LookupContext::onDone() {
    finished_ = true;
    for (const auto& [id, pi] : closest_peers_) {
        node_.insert(pi);
    }
    // fmt::println("!!! going to erase '{}' context", nonce_);
    peer_.endContext(nonce_);
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

void LookupContext::maybeFinish() {
    if (finished_) {
        return;
    }
    auto   start        = closest_peers_.begin();
    size_t closest_size = std::min(kReturn, closest_peers_.size());
    auto   closest_end  = start;
    std::advance(closest_end, closest_size);

    std::map<NodeId, PeerInfo, Comparator> chosen_peers(comp_);
    chosen_peers.insert(start, closest_end);

    if (shouldStop(chosen_peers)) {
        onDone();
    }
    if (inflight_ == 0 && queried_.size() >= kReturn) {
        onDone();
    }
}
