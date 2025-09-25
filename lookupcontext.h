#pragma once

// #include "inode.h"
// #include "ipeer.h"
#include "utils.h"
#include <functional>
#include <map>
#include <memory>
#include <set>

using Comparator = std::function<bool(const Id& a, const Id& b)>;
class IPeer;
class INode;
class LookupStats;

class LookupContext : public std::enable_shared_from_this<LookupContext> {
   private:
    Id       target_;
    IPeer&   peer_;
    INode&   node_;
    uint64_t nonce_ = 0;

    Comparator comp_;

    std::set<Id, Comparator>                                 queried_;
    uint32_t                                                 launched_;
    std::map<Id, PeerInfo, Comparator>                       closest_peers_;
    std::vector<PeerInfo>                                    final_result_;
    std::map<Id, std::shared_ptr<boost::asio::steady_timer>> timers_;
    std::shared_ptr<LookupStats>                             stats_;

    size_t inflight_ = 0;
    bool   finished_ = false;

    void issueNext();
    void maybeFinish();

   public:
    LookupContext(Id                           target,
                  IPeer&                       peer,
                  INode&                       node,
                  uint64_t                     nonce,
                  std::shared_ptr<LookupStats> stats)
      : target_(target)
      , peer_(peer)
      , node_(node)
      , nonce_(nonce)
      , comp_([target](const Id& a, const Id& b) {
          return distance(a, target) < distance(b, target);
      })
      , queried_(comp_)
      , closest_peers_(comp_)
      , stats_(std::move(stats)){};

    void start();
    void sendFindNodeQuery(const PeerInfo& pi);
    void onDone();
    bool shouldStop();
    void startQueryTimer(const Id& whom);

    void onResponse(const Id& id, std::vector<PeerInfo> result);
};
