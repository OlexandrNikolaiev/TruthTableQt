#include <QApplication>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString openSansSemiBold = ":/fonts/fonts/OpenSans-SemiBold.ttf";
    QFontDatabase::addApplicationFont(openSansSemiBold);

    MainWindow w;
    w.show();

    return a.exec();
}
