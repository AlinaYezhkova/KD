#include "MainWin.h"
#include "test.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWin w;
    // w.show();
    // return a.exec();

    Test::SwarmBootstrap();

    // Test::swarmSearch();

    // Test::id();

    return 0;
}

