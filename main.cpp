#include "MainWin.h"
#include "test.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWin w;
    // w.show();
    // return a.exec();

    test::SwarmBootstrap();

    // Test::swarmSearch();

    // Test::id();

    // Test::poolTest();

    return 0;
}

