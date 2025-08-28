// #include <QApplication>
#include "peer.h"
#include <fstream>

int main(int argc, char* argv[]) /*try*/ {
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
    std::vector<PeerInfo>              boot_nodes;
    std::vector<std::shared_ptr<Peer>> peers;

    auto boot_peer =
        std::make_shared<Peer>(io, std::vector<PeerInfo>{}, host, 5000);
    peers.push_back(boot_peer);

    boot_nodes.push_back(boot_peer->getPeerInfo());

    for (int i = 0; i < 20; ++i) {
        peers.push_back(std::make_shared<Peer>(io, boot_nodes, host));
    }

    boost::asio::signal_set signals(io, SIGINT, SIGTERM, SIGHUP);
    signals.async_wait(
        [&](const boost::system::error_code&, int signal_number) {
            fmt::println(" Received signal {}, stopping peer", signal_number);
            io.stop();
        });

    io.run();

    return 0;
} 
// catch (const std::exception& e) {
//     fmt::println(stderr, "Exception in main: {}", e.what());
//     return 1;
// }
