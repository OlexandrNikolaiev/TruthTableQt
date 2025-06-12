#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QTranslator>

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


    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "cursova_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            qDebug()<<"123";
            a.installTranslator(&translator);
            break;
        }
    }
    registerFileAssociation();

    QString openSansSemiBold = QCoreApplication::applicationDirPath() + "/fonts/OpenSans-SemiBold.ttf";
    QFontDatabase::addApplicationFont(openSansSemiBold);

    MainWindow w;


    w.show();

    return a.exec();
}
