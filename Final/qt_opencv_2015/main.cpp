#include "mainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainWindow w(0);
    w.show();
    
    return a.exec();
}
