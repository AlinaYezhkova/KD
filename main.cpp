#include "MainWin.h"
#include "id.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWin w;
    // w.show();

    Id::testId();

    return a.exec();
}
