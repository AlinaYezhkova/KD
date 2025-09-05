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

    boost::asio::io_context            io;
    std::string                        host = "127.0.0.1";
    std::vector<std::shared_ptr<Peer>> peers;
    Swarm&                             swarm = Swarm::getInstance();
    auto                               guard = boost::asio::make_work_guard(io);

    int numThreads = std::max(1, (int) sysconf(_SC_NPROCESSORS_ONLN));
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&] { io.run(); });
    }

    auto boot_peer = std::make_shared<Peer>(io, host, 5000);
    peers.push_back(boot_peer);
    boot_peer->start(true);
    swarm.add(boot_peer);

    for (int i = 0; i < 300; ++i) {
        auto peer = std::make_shared<Peer>(io, host);
        peers.push_back(peer);
        peer->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    boost::asio::signal_set signals(io, SIGINT, SIGTERM, SIGHUP);
    signals.async_wait(
        [&](const boost::system::error_code&, int signal_number) {
            fmt::println(" Received signal {}, stopping peer", signal_number);
            io.stop();
        });

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
