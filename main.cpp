#include "lookupStats.h"
#include "peer.h"
#include "swarm.h"
#include <fstream>

int main(int argc, char* argv[]) {
    // std::ofstream fs(g_file_path);
    // if (fs) {
    //     fs.clear();
    //     fs.close();
    // } else {
    //     fmt::println("not cleared");
    // }

    boost::asio::io_context io;
    std::string             host = "127.0.0.1";
    Swarm::init_swarm(io);
    auto& swarm = Swarm::getInstance();
    auto  guard = boost::asio::make_work_guard(io);

    int num_threads = std::max(1, (int) sysconf(_SC_NPROCESSORS_ONLN));
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&] { io.run(); });
    }
    auto stats = std::make_shared<LookupStats>();

    bool is_boot_node = true;
    auto boot_peer =
        std::make_shared<Peer>(io, host, stats, 5000, is_boot_node);
    boot_peer->start();
    swarm.add(boot_peer);

    for (int i = 0; i < 1024; ++i) {
        auto peer = std::make_shared<Peer>(io, host, stats);
        peer->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    for (int i = 0;; ++i) {
        fmt::println("-----------------------round {}-----------------------",
                     i + 1);

        swarm.async_for_each_peer([&](std::shared_ptr<IPeer> peer) {
            swarm.async_getRandomPeer([peer = std::move(peer)](
                                          std::shared_ptr<IPeer> target) {
                if (!target) {
                    return;
                }
                // avoid self-target if needed:
                if (target->getPeerInfo().key_ == peer->getPeerInfo().key_) {
                    return;
                }
                // IMPORTANT: call Peer API in its own strand (or its
                // methods already do so)
                peer->find(target->getPeerInfo().key_);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            });
        });
        std::this_thread::sleep_for(std::chrono::seconds(10));
        uint64_t found_nodes = stats->getFoundNodes();
        uint64_t total_hops  = stats->getTotalHopCounts();
        double   avg_hops    = total_hops / (double) found_nodes;
        fmt::println("Total found: \t {}", found_nodes);
        fmt::println("Avg hops: \t {}", avg_hops);
        stats->resetHopCount();
        stats->resetFoundNodes();
        stats->resetTotalHopCounts();
    }

    boost::asio::signal_set signals(io, SIGINT, SIGTERM, SIGHUP);
    signals.async_wait(
        [&](const boost::system::error_code&, int signal_number) {
            fmt::println(" Received signal {}, stopping peer", signal_number);
            io.stop();
            google::protobuf::ShutdownProtobufLibrary();
        });

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
