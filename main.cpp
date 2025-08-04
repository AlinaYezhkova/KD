#include "swarm.h"
// #include <QApplication>
#include <fstream>

int main(int argc, char* argv[]) {
    // QApplication a(argc, argv);
    // MainWin w;
    // w.show();
    // return a.exec();
    std::ofstream fs("log.txt");
    if (fs) {
        fs.clear();
        fs.close();
    }

    boost::asio::io_context io;

    auto swarm = std::make_shared<Swarm>(io);
    swarm->addNode(Id(0), true);
    for (int i = 1; i < g_boot_number; ++i) {
        swarm->addNode(Id(i));
    }
    fmt::println("Swarm size: {}", swarm->nodes().size());

    swarm->bootstrapAll();

    // for(auto&[id, node] : swarm->nodes()) {
    //     node->print();
    // }
    swarm->startPeriodicLookups(std::chrono::seconds(g_lookup_interval));
    // swarm->printStatistics();
    std::vector<std::thread> threads;
    int                      num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_threads <= 0) {
        num_threads = 8;
    }
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&io]() { io.run(); });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
}
