#include "lookupcontext.h"

#include "constants.h"
#include "inode.h"
#include "ipeer.h"
#include "lookupStats.h"
#include "messageBuilder.h"

void LookupContext::start() {
    std::vector<PeerInfo> closest = node_.find_K_closest(target_);
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
        if (!queried_.contains(id)) {
            unqueried.push_back(pi);
        }
    }

    if (unqueried.empty() && inflight_ == 0) {
        onDone();
        return;
    }

    int launched = 0;

    for (const auto& pi : unqueried) {
        if (pi.key_ == peer_.getPeerInfo().key_) {
            continue;
        }
        queried_.insert(pi.key_);
        ++inflight_;
        node_.insert(pi);
        sendFindNodeQuery(pi);
        // startQueryTimer(pi.key_);
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

void LookupContext::onResponse(const Id&             id,
                               std::vector<PeerInfo> peers_found) {
    if (inflight_ > 0) {
        --inflight_;
    }
    // if (auto it = timers_.find(id); it != timers_.end()) {
    //     it->second->cancel();
    //     timers_.erase(it);
    // }

    for (const auto& pi : peers_found) {
        closest_peers_[pi.key_] = pi;
    }
    issueNext();
}

void LookupContext::onDone() {
    finished_ = true;
    // for (auto& [_, t] : timers_) t->cancel();
    // timers_.clear();

    int i = 0;
    for (const auto& [id, pi] : closest_peers_) {
        node_.insert(pi);
        if (++i >= kReturn) {
            break;
        }
    }
    peer_.endContext(nonce_);
}

bool LookupContext::shouldStop() {
    if (closest_peers_.count(target_) > 0) {
        stats_->addFoundNode();
        return true;
    }
    return closest_peers_.size() >= kReturn;
}

void LookupContext::maybeFinish() {
    if (shouldStop()) {
        onDone();
    }
    if (inflight_ == 0 && queried_.size() >= kReturn) {
        onDone();
    }
}

void LookupContext::startQueryTimer(const Id& whom) {
    auto timer = std::make_shared<boost::asio::steady_timer>(peer_.getStrand());
    timer->expires_after(std::chrono::milliseconds(100));
    auto nonce = nonce_;
    timer->async_wait(boost::asio::bind_executor(
        peer_.getStrand(),
        [this, timer, whom, nonce](const boost::system::error_code& ec) {
            if (ec || finished_) {
                return;
            }
            // treat as no-response
            if (queried_.contains(whom)) {
                if (inflight_ > 0) {
                    --inflight_;
                }
                maybeFinish();
                if (!finished_) {
                    issueNext();
                }
            }
        }));

    timers_[whom] = timer;
}