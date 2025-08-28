#pragma once

#include "inode.h"
#include "lookupcontext.h"
#include "message.pb.h"
#include <boost/asio.hpp>
#include <optional>

using udp = boost::asio::ip::udp;

class IPeer : public std::enable_shared_from_this<IPeer> {
   public:
    virtual const PeerInfo& getPeerInfo() = 0;

    virtual void receiveLoop() = 0;

    virtual void bootstrap() = 0;

    virtual void onDatagram(const uint8_t*       data,
                            std::size_t          bytes_received,
                            const udp::endpoint& from) = 0;

    virtual void send(const Message& m)    = 0;
    virtual void endLookup(uint64_t nonce) = 0;

    virtual void handleFindNodeQuery(const Message& msg) = 0;
    virtual void handleFindNodeReply(const Message& msg) = 0;
    // virtual std::shared_ptr<LookupContext> createLookupContext(
    //     const PeerInfo& sender,
    //     NodeId          target,
    //     uint64_t        nonce) = 0;
};