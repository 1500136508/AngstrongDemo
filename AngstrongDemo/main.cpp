#include "AngstrongDemo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AngstrongDemo w;
    w.show();
    return a.exec();
}
