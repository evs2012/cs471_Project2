#include "memoryapplication.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MemoryApplication w;
    w.show();

    return a.exec();
}
