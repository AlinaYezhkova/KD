#pragma once

#include <memory>
#include <random>
#include <vector>

class IPeer;

class Swarm : public std::enable_shared_from_this<Swarm> {
   private:
    std::vector<std::shared_ptr<IPeer>> peers_;
    std::random_device                  rd_;

    Swarm() {}
    Swarm(Swarm const&)            = delete;
    Swarm& operator=(Swarm const&) = delete;

   public:
    static Swarm& getInstance() {
        static Swarm s;
        return s;
    }
    void add(const std::shared_ptr<IPeer>& peer) { peers_.push_back(peer); }

    std::shared_ptr<IPeer> getRandomPeer();
};