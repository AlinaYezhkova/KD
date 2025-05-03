#include "MainWin.h"
#include "test.h"
#include <fstream>
#include "node.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWin w;
    // w.show();
    // return a.exec();
    std::ofstream fs("log.txt");
    if(fs)
    {
        fs.clear();
        fs.close();
    }

    test::SwarmBootstrap();

    // Test::swarmSearch();

    // test::id();

    // Test::poolTest();

    return 0;
}

