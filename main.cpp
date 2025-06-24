#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QTranslator>
#include <windows.h>

#include "mainwindow.h"

QColor getWindowsBackgroundColor() {
    HKEY hKey;
    const char* subKey = R"(Control Panel\Colors)";
    const char* valueName = "Background";
    char value[255];
    DWORD valueLen = sizeof(value);
    DWORD type = REG_SZ;

    if (RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        qWarning() << "Cannot open registry key.";
        return QColor(Qt::black); // fallback
    }

    if (RegQueryValueExA(hKey, valueName, nullptr, &type, (LPBYTE)value, &valueLen) != ERROR_SUCCESS) {
        qWarning() << "Cannot read registry value.";
        RegCloseKey(hKey);
        return QColor(Qt::black); // fallback
    }

    RegCloseKey(hKey);

    int r, g, b;
    if (sscanf_s(value, "%d %d %d", &r, &g, &b) != 3) {
        qWarning() << "Failed to parse background color.";
        return QColor(Qt::black); // fallback
    }

    QColor backgroundColor(r, g, b);
    qDebug() << "Windows background color:" << backgroundColor;
    return backgroundColor;
}

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

    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1) {
        QString filePath = args.at(1);
        w.loadFile(filePath);
    }

    w.show();

    // QColor rgb = getWindowsBackgroundColor();
    // w.applyBackgroundColorToMenuBar(rgb);


    return a.exec();
}
