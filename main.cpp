// #include <QApplication>
#include "peer.h"
#include "swarm.h"
#include <fstream>

int main(int argc, char* argv[]) {
    // QApplication a(argc, argv);
    // MainWin w;
    // w.show();
    // return a.exec();

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

    int numThreads = std::max(1, (int) sysconf(_SC_NPROCESSORS_ONLN));
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&] { io.run(); });
    }

    bool isBootNode = true;
    auto boot_peer  = std::make_shared<Peer>(io, host, 5000, isBootNode);
    boot_peer->start();
    swarm.add(boot_peer);

    for (int i = 0; i < 30; ++i) {
        auto peer = std::make_shared<Peer>(io, host);
        peer->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    // for (int i = 0; i < 10; ++i) {
    //     swarm.async_for_each_peer([&](std::shared_ptr<IPeer> peer) {
    //         swarm.async_getRandomPeer(
    //             [peer = std::move(peer)](std::shared_ptr<IPeer> target) {
    //                 if (!target) {
    //                     return;
    //                 }
    //                 // avoid self-target if needed:
    //                 // if (target->getPeerInfo().key_ ==
    //                 // peer->getPeerInfo().key_)
    //                 //     return;
    //                 // IMPORTANT: call Peer API in its own strand (or its
    //                 // methods already do so)
    //                 peer->find(target->getPeerInfo().key_);
    //                 std::this_thread::sleep_for(std::chrono::milliseconds(3));
    //             });
    //     });
    //     std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // }

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
