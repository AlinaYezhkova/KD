#include "lookupcontext.h"

#include "constants.h"
#include "utils.h"

void LookupContext::start() {
    auto my_closest = node.getClosestKnownNodes(
        target);  // g_pool_size or fewer; only 0 at bootstrap
    closest.insert(my_closest.begin(), my_closest.end());
    postNext();
}

void LookupContext::postNext() {
    if (stopWhenFound && foundTarget)
        return;
    boost::asio::post(node.getStrand(),
                      [self = shared_from_this()] { self->continueLookup(); });
}

void LookupContext::continueLookup() {
    /*
    The lookup stops early if:
    You enabled stopWhenFound and the target was already found;
    You've already queried at least g_pool_size nodes;
    Too many lookups are currently in flight.
    */
    if (queried.size() >= g_pool_size || inFlight > g_alpha)
        return;

    int started = 0;

    /*
    Loop over closest nodes and:
    Skip already queried nodes;
    Limit to g_alpha parallel requests per round;
    Mark each as queried;
    Increment inFlight and started.
    */
    for (const auto& id : closest) {
        if (queried.count(id))
            continue;
        if (started >= g_alpha)
            break;

        queried.insert(id);

        ++inFlight;
        ++started;

        /*
        For each selected node:
        Send an asynchronous FIND_NODE RPC;
        When the node replies with a list of Ids (supposedly closer to the
        target), pass it to handleResponse().
        */

        node.sendFindNodeRPC(
            id,
            target,
            [self = shared_from_this(), id](std::vector<Id> result) {
                self->handleResponse(id, result);
            });
    }

    /*
    We didn’t start any new requests (started == 0);
    No requests are in flight (inFlight == 0);
    And we didn’t break early due to already finding the target.
    */

    // node.scheduleRefresh();
}

void LookupContext::handleResponse(Id from, const std::vector<Id>& nodes) {
    boost::asio::post(
        node.getStrand(),
        [self = shared_from_this(), from, nodes = std::vector<Id>(nodes)]() {
            for (const auto& id : nodes) {
                self->closest.insert(id);
                self->node.insertNode(id);
                if (self->stopWhenFound && id == self->target) {
                    self->foundTarget = true;
                    if (self->completionCallback_) {
                        self->completionCallback_(self->foundTarget);
                        self->completionCallback_ = nullptr;
                    }
                    // fmt::println("[{}] Found target node {}!",
                    //              toBinaryString(self->node.getId()),
                    //              toBinaryString(id));
                    break;
                }
            }

            --self->inFlight;
            
            // self->closest.erase(self->closest.find(from));
            self->postNext();
        });
}
