#include "swarm.h"

#include <fmt/core.h>

#include "peer.h"
std::shared_ptr<IPeer> Swarm::getRandomPeer() {
    std::mt19937                       generator(rd_());
    std::uniform_int_distribution<int> distribution(0, peers_.size() - 1);
    auto                               sth = peers_.size();
    fmt::println("peers_.size() = {}", sth);
    // for (auto e : peers_) {
    //     fmt::println("{}", e->getPeerInfo().key_);
    // }
    auto nado = distribution(generator);
    // fmt::println("nado = {}", nado);

    return peers_[nado];
}