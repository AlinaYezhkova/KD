#pragma once

#include "inode.h"
#include <boost/asio.hpp>
#include <set>
#include <unordered_set>

struct LookupContext : public std::enable_shared_from_this<LookupContext> {
    INode&                 node;
    const Id               target;
    std::unordered_set<Id> queried;
    using Comparator = std::function<bool(const Id& a, const Id& b)>;
    std::set<Id, Comparator>  closest;
    // int                       inFlight      = 0;
    bool                      stopWhenFound = false;
    bool                      foundTarget   = false;
    std::function<void(bool)> completionCallback_;

    void setStopWhenFound(bool b) { stopWhenFound = b; }

    LookupContext(INode& n, const Id& t)
      : node(n), target(t), closest([&](const Id& a, const Id& b) {
          return (a ^ target) < (b ^ target);
      }) {}

    void start();
    void postNext();
    void continueLookup();
    void handleResponse(const Id& from, const std::vector<Id>& nodes);
    void setCompletionCallback(std::function<void(bool)> cb) {
        completionCallback_ = std::move(cb);
    }
};