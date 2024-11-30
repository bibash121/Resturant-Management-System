#include "rms.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RMS w;
    w.show();
    return a.exec();
}
