#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("GoonSoft");
    a.setApplicationName("V3");

    MainWindow w;
    w.show();

    return a.exec();
}
