#pragma once

#include "constants.h"
#include "id.h"
#include "message.pb.h"
#include <array>
#include <boost/asio.hpp>
#include <optional>

using udp = boost::asio::ip::udp;
class LookupContext;
class INode;
class LookupStats;
static constexpr std::size_t MAX_DGRAM = 1200;  // safe under typical MTU

class IPeer : public std::enable_shared_from_this<IPeer> {
   public:
    virtual void receiveLoop() = 0;

    virtual void bootstrap()                = 0;
    virtual void find(const Id& id)         = 0;
    virtual void insert(const PeerInfo& pi) = 0;

    virtual void start() = 0;

    virtual void onDatagram(const uint8_t*       data,
                            std::size_t          bytes_received,
                            const udp::endpoint& from) = 0;

    virtual void send(const Message& m)                           = 0;
    virtual void startContext(uint64_t                       nonce,
                              std::shared_ptr<LookupContext> ctx) = 0;
    virtual void endContext(uint64_t nonce)                       = 0;

    virtual const PeerInfo&                       getPeerInfo()       = 0;
    virtual boost::asio::io_context&              getIo()             = 0;
    virtual const std::array<uint8_t, MAX_DGRAM>& getBuffer()         = 0;
    virtual const udp::endpoint&                  getSender()         = 0;
    virtual const std::unique_ptr<INode>&         getNode() const     = 0;
    virtual std::shared_ptr<LookupContext> getContext(uint64_t nonce) = 0;
    virtual std::shared_ptr<LookupStats>   getStats()                 = 0;

    virtual const boost::asio::strand<boost::asio::io_context::executor_type>&
    getStrand() = 0;

    virtual ~IPeer() = default;
};