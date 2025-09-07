#pragma once

#include "constants.h"
#include "inode.h"
#include "peer.h"
#include <functional>
#include <memory>
#include <random>
#include <vector>

class IPeer;

// class Swarm : public std::enable_shared_from_this<Swarm> {
//    private:
//     std::vector<std::shared_ptr<IPeer>> peers_;
//     std::random_device                  rd_;

//     Swarm() {}
//     Swarm(Swarm const&)            = delete;
//     Swarm& operator=(Swarm const&) = delete;

//    public:
//     static Swarm& getInstance() {
//         static Swarm s;
//         return s;
//     }
//     void add(const std::shared_ptr<IPeer>& peer) { peers_.push_back(peer); }

//     std::shared_ptr<IPeer> getRandomPeer();
//     std::shared_ptr<IPeer> getClosestPeer(const NodeId& id);
// };

class Swarm {
   private:
    static Swarm*& instance() {
        static Swarm* p = nullptr;
        return p;
    }

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    std::vector<std::shared_ptr<IPeer>>                         peers_;
    std::mt19937                                                rng_;

   public:
    explicit Swarm(boost::asio::io_context& io)
      : strand_(boost::asio::make_strand(io)), rng_(std::random_device{}()) {}

    static Swarm& getInstance() {
        Swarm* p = instance();
        if (!p) {
            throw std::logic_error("call init_swarm(io) first");
        }
        return *p;
    }
    static void init_swarm(boost::asio::io_context& io) {
        static Swarm inst(io);
        instance() = &inst;
    }

    void add(std::shared_ptr<IPeer> peer) {
        boost::asio::post(strand_, [this, peer = std::move(peer)] {
            peers_.push_back(std::move(peer));
        });
    }

    template <class Handler> void async_getRandomPeer(Handler h) {
        boost::asio::dispatch(strand_, [this, h = std::move(h)]() mutable {
            auto sth = peers_.size();
            if (sth % 10 == 0) {
                fmt::println("peers_.size() = {}", sth);
            }
            std::shared_ptr<IPeer>                out;
            std::uniform_int_distribution<size_t> dist(0, peers_.size() - 1);
            out = peers_[dist(rng_)];
            h(std::move(out));
        });
    }

    template <class Handler>
    void async_getClosestPeer(const NodeId& id, Handler h) {
        boost::asio::dispatch(strand_, [this, id, h = std::move(h)]() mutable {
            auto sth = peers_.size();
            if (sth % 10 == 0) {
                fmt::println("peers_.size() = {}", sth);
            }

            std::shared_ptr<IPeer> out;
            // pick best by XOR distance, tiebreak by last_seen_
            size_t best    = 0;
            auto   best_pi = peers_[0]->getPeerInfo();
            auto   best_d  = distance(id, best_pi.key_);

            // fmt::println("{} compared to {} = {}", id, best_pi.key_, best_d);

            for (size_t i = 1; i < peers_.size(); ++i) {
                auto pi = peers_[i]->getPeerInfo();
                auto d  = distance(id, pi.key_);
                // fmt::println("{} compared to {} = {}", id, pi.key_, d);

                if (d < best_d ||
                    (d == best_d && pi.last_seen_ < best_pi.last_seen_)) {
                    best    = i;
                    best_d  = d;
                    best_pi = pi;
                }
            }
            out = peers_[best];
            // fmt::println("{} is the best", out->getPeerInfo().key_);
            // fmt::println("");

            h(std::move(out));  // invoked on Swarm's strand
        });
    }
};