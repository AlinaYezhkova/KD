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
        std::make_shared<Peer>(io, host, 0, stats, 5000, is_boot_node);
    boot_peer->start();
    swarm.add(boot_peer);

    for (int i = 1; i <= kSwarmSize; ++i) {
        auto peer = std::make_shared<Peer>(io, host, i, stats);
        peer->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
    fmt::println("swarm size = {}", swarm.getPeers().size());

    fmt::println("you should reach convergence at {} hops",
                 (1 / harmonic(kBucketSize)) * std::log2(kSwarmSize));

    for (int i = 0;; ++i) {
        fmt::println("-----------------------round {}-----------------------",
                     i + 1);
        auto queries_sent = std::make_shared<std::size_t>(0);

        auto find_handler = [queries_sent](std::shared_ptr<IPeer> caller_peer) {
            return [caller_peer, queries_sent](std::shared_ptr<IPeer> target) {
                if (!target) {
                    return;
                }
                if (target->getPeerInfo().key_ ==
                    caller_peer->getPeerInfo().key_) {
                    return;
                }
                ++(*queries_sent);
                caller_peer->find(target->getPeerInfo().key_);
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(kMsBetweenPeers));
            };
        };

        swarm.async_for_each_peer([&](std::shared_ptr<IPeer> peer) {
            auto handler = find_handler(peer);
            fmt::println("{}", peer->getPeerInfo().key_.getBits().to_string());
            swarm.async_getRandomPeer(handler);
            // swarm.async_getOppositePeer(peer, handler);
        });
        std::this_thread::sleep_for(
            std::chrono::milliseconds(kMsBetweenSearches));
        uint64_t found_nodes = stats->getFoundNodes();
        uint64_t total_hops  = stats->getTotalHopCounts();
        double   avg_hops    = total_hops / (double) found_nodes;
        fmt::println("Total found: \t {} (out of {})",
                     found_nodes,
                     *queries_sent);
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
