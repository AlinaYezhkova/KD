#include "MainWin.h"
#include "swarm.h"
#include "test.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWin w;
    // w.show();
    // return a.exec();

    Swarm swarm;
    swarm.initiate();
    std::cout << swarm;
    return 0;


}

