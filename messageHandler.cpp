#include "messageHandler.h"

#include "messageBuilder.h"
#include "peer.h"
#include "swarm.h"

void MessageHandler::handleBootstrapQuery(IPeer& peer, const Message& msg) {
    PeerInfo sender  = peerInfoFromProto(msg.from_user());
    bool     success = peer.getNode()->insert(sender);
    Message  reply   = MessageBuilder()
                        .type(MessageType::Bootstrap_reply)
                        .from(peer.getPeerInfo())
                        .to(sender)
                        .booted(success)
                        .build();
    peer.send(reply);
}

void MessageHandler::handleBootstrapReply(IPeer& peer, const Message& msg) {
    if (!msg.bootstrapped()) {
        peer.bootstrap();
    } else {
        Swarm& swarm = Swarm::getInstance();
        swarm.add(peer.shared_from_this());
    }
}

void MessageHandler::handleFindNodeQuery(IPeer& peer, const Message& msg) {
    PeerInfo sender = peerInfoFromProto(msg.from_user());
    Id       target = idFromProto(msg.find_user());
    uint64_t nonce  = msg.nonce();

    peer.insert(sender);
    std::vector<PeerInfo> closest = peer.getNode()->find_K_closest(target);

    Message response = MessageBuilder()
                           .type(MessageType::Find_node_reply)
                           .from(peer.getPeerInfo())
                           .to(sender)
                           .result(closest)
                           .nonce(nonce)
                           .build();

    peer.send(response);
}

void MessageHandler::handleFindNodeReply(IPeer& peer, const Message& msg) {
    auto ctx = peer.getContext(msg.nonce());
    if (!ctx) {
        // throw std::runtime_error("wrong nonce or context unavailable");
        return;
    }
    std::vector<PeerInfo> result = resultFromProto(msg);
    ctx->onResponse(peer.getPeerInfo().key_, result);
}

void MessageHandler::handle(IPeer& peer, const Message& msg) {
    switch (msg.type()) {
        case Find_node_query:
            handleFindNodeQuery(peer, msg);
            break;

        case Find_node_reply:
            handleFindNodeReply(peer, msg);
            break;

        case Bootstrap_query:
            handleBootstrapQuery(peer, msg);
            break;

        case Bootstrap_reply:
            handleBootstrapReply(peer, msg);
            break;

        default:
            std::cerr << "Unknown msg type\n";
    }
}