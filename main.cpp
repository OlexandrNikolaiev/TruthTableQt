#include <QApplication>
#include <QFontDatabase>
#include "mainwindow.h"

void registerFileAssociation()
{
    QString appPath = QCoreApplication::applicationFilePath().replace("/", "\\");
    qDebug()<<appPath;

    QSettings settings("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
    settings.setValue(".builder/.", "BuilderFile");
    settings.setValue("BuilderFile/.", "Builder File");
    settings.setValue("BuilderFile/shell/open/command/.", "\"" + appPath + "\" \"%1\"");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    registerFileAssociation();

    QString openSansSemiBold = ":/fonts/fonts/OpenSans-SemiBold.ttf";
    QFontDatabase::addApplicationFont(openSansSemiBold);


    MainWindow w;
    a.setStyleSheet("QPushButton{color:red;};");
    w.show();

    return a.exec();
}
