// #include "swarm.h"

// #include <fmt/core.h>

// #include <set>

// std::shared_ptr<IPeer> Swarm::getClosestPeer(const NodeId& id) {
//     auto cmp = [id](const std::shared_ptr<IPeer>& a,
//                     const std::shared_ptr<IPeer>& b) {
//         auto da = xor_id(id, a->getPeerInfo().key_);
//         auto db = xor_id(id, b->getPeerInfo().key_);
//         if (da != db) {
//             return da < db;
//         }
//         return a->getPeerInfo().last_seen_ < b->getPeerInfo().last_seen_;
//     };

//     std::set<std::shared_ptr<IPeer>, decltype(cmp)> sortedPeers(cmp);
//     sortedPeers.insert(peers_.begin(), peers_.end());
//     return peers_[0];
//     // return *sortedPeers.begin();
// }
