#pragma once

#include "constants.h"
#include "inode.h"
#include "peer.h"
#include <functional>
#include <memory>
#include <random>
#include <vector>

class IPeer;

class Swarm {
   private:
    static Swarm*& instance() {
        static Swarm* p = nullptr;
        return p;
    }

    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    std::vector<std::shared_ptr<IPeer>>                         peers_;
    std::unordered_map<Id, std::shared_ptr<IPeer>>              peers_map_;
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

    std::vector<std::shared_ptr<IPeer>> getPeers() {
        return getInstance().peers_;
    }

    static void init_swarm(boost::asio::io_context& io) {
        static Swarm inst(io);
        instance() = &inst;
    }

    void add(std::shared_ptr<IPeer> peer) {
        boost::asio::post(strand_, [this, peer = std::move(peer)] {
            peers_map_.emplace(peer->getPeerInfo().key_, peer);
            peers_.push_back(std::move(peer));
        });
    }

    template <class Handler> void async_getRandomPeer(Handler h) {
        boost::asio::dispatch(strand_, [this, h = std::move(h)]() mutable {
            std::shared_ptr<IPeer>                out;
            std::uniform_int_distribution<size_t> dist(0, peers_.size() - 1);
            out = peers_[dist(rng_)];
            fmt::println("{}", out->getPeerInfo().key_.getBits().to_string());
            fmt::println("-------------");
            h(std::move(out));
        });
    }

    template <class Handler>
    void async_getClosestPeer(const Id& id, Handler h) {
        boost::asio::dispatch(strand_, [this, id, h = std::move(h)]() mutable {
            std::shared_ptr<IPeer> out;
            size_t                 best    = 0;
            auto                   best_pi = peers_[0]->getPeerInfo();
            auto                   best_d  = distance(id, best_pi.key_);

            for (size_t i = 1; i < peers_.size(); ++i) {
                auto pi = peers_[i]->getPeerInfo();
                auto d  = distance(id, pi.key_);
                if (d < best_d ||
                    (d == best_d && pi.last_seen_ < best_pi.last_seen_)) {
                    best    = i;
                    best_d  = d;
                    best_pi = pi;
                }
            }
            out = peers_[best];
            h(std::move(out));  // invoked on Swarm's strand
        });
    }

    template <class Handler>
    void async_getOppositePeer(const std::shared_ptr<IPeer>& peer, Handler h) {
        boost::asio::dispatch(
            strand_,
            [this, p = peer, h = std::move(h)]() mutable {
                Id target_id               = opposite(p->getPeerInfo().key_);
                std::shared_ptr<IPeer> out = nullptr;
                if (auto it = peers_map_.find(target_id);
                    it != peers_map_.end()) {
                    out = it->second;  // no insertion, no nullptr surprise
                    h(std::move(out));
                }
            });
    }

    template <class F> void async_for_each_peer(F f) {
        boost::asio::dispatch(strand_, [this, f = std::move(f)]() mutable {
            for (auto& p : peers_) f(p);  // p is std::shared_ptr<IPeer>
        });
    }
};