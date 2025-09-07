#pragma once

#include "constants.h"
#include "inode.h"
#include "lookupcontext.h"
#include "message.pb.h"
#include <array>
#include <boost/asio.hpp>
#include <optional>

using udp                              = boost::asio::ip::udp;
static constexpr std::size_t MAX_DGRAM = 1200;  // safe under typical MTU

class IPeer : public std::enable_shared_from_this<IPeer> {
   public:
    virtual void receiveLoop() = 0;

    virtual void bootstrap() = 0;

    virtual void start() = 0;

    virtual void onDatagram(const uint8_t*       data,
                            std::size_t          bytes_received,
                            const udp::endpoint& from) = 0;

    virtual void send(const Message& m)    = 0;
    virtual void endLookup(uint64_t nonce) = 0;

    virtual void handleFindNodeQuery(const Message& msg)  = 0;
    virtual void handleFindNodeReply(const Message& msg)  = 0;
    virtual void handleBootstrapQuery(const Message& msg) = 0;
    virtual void handleBootstrapReply(const Message& msg) = 0;

    virtual const PeerInfo&                       getPeerInfo() = 0;
    virtual const std::array<uint8_t, MAX_DGRAM>& getBuffer()   = 0;
    virtual const udp::endpoint&                  getSender()   = 0;

    virtual const boost::asio::strand<boost::asio::io_context::executor_type>&
    getStrand() = 0;

    virtual const std::unique_ptr<INode>& getNode() const = 0;

    // virtual std::shared_ptr<LookupContext> createLookupContext(
    //     const PeerInfo& sender,
    //     NodeId          target,
    //     uint64_t        nonce) = 0;
    virtual ~IPeer() = default;
};