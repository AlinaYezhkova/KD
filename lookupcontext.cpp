#include "lookupcontext.h"

#include "constants.h"
#include "utils.h"

void LookupContext::start() {
    auto vec = node.getClosestKnownNodes(target);  // g_pool_size or fewer
    closest.insert(vec.begin(), vec.end());
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
    if (queried.size() >= g_pool_size
        // ||inFlight > g_alpha
    )
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

        // insert myself into a closest node, for i already know it (
        // from LookupContext::start() )
        node.getNode(id)->asyncInsertNode(node.getId());
        // node.getNode(id)->insertNode(node.getId());
        // ++inFlight;
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
    // if (started == 0 && inFlight == 0 && !(stopWhenFound && foundTarget)) {
    // if (started == 0 && !(stopWhenFound && foundTarget)) {
    //     fmt::println("[{}] Node lookup for {} complete. Queried {} nodes.",
    //                  toBinaryString(node.getId()),
    //                  toBinaryString(target),
    //                  queried.size());
    // }
    // node.scheduleRefresh();
}

void LookupContext::handleResponse(const Id&              from,
                                   const std::vector<Id>& nodes) {
    boost::asio::post(node.getStrand(), [self = shared_from_this(), nodes]() {
        for (const auto& id : nodes) {
            self->closest.insert(id);
            self->node.insertNode(id);
            self->node.getNode(id)->asyncInsertNode(self->node.getId());
            // self->node.getNode(id)->insertNode(self->node.getId());
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

        // --self->inFlight;
        self->postNext();
    });
}
