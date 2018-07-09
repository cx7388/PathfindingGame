#include "mainwindow.h"
#include <QApplication>
#include "world.h"
#include <QString>
#include <QFile>
#include <string>
#include <vector>
#include <QtGui>
#include "pathfinder.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
