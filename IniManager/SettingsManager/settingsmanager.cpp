#include "settingsmanager.h"
#include <QColor>

SettingsManager::SettingsManager(const QString& fileName)
    : IniManager(fileName)
{
}

void SettingsManager::saveWindowGeometry(const QByteArray& geometry)
{
    saveValue("Window", "geometry", geometry);
}

QByteArray SettingsManager::loadWindowGeometry()
{
    return loadValue("Window", "geometry").toByteArray();
}

void SettingsManager::saveShowAuxButtons(bool show)
{
    saveValue("MainWindow", "showAuxButtons", show);
}

bool SettingsManager::loadShowAuxButtons()
{
    return loadValue("MainWindow", "showAuxButtons").toBool();
}

void SettingsManager::saveCellHoverColor(const QColor &color)
{
    qDebug()<<"saving "<<color;
    saveValue("Table", "cellHoverColor", color);
}

QString SettingsManager::loadCellHoverColor()
{
    QVariant value = loadValue("Table", "cellHoverColor");
    if (!value.isValid() || value.isNull()) {
        qDebug() << "No valid color found, returning invalid QColor";
        return QString();
    }


    QString hexColor = value.value<QString>();
    return hexColor;
}

void SettingsManager::saveLanguage(QString language)
{
    saveValue("Language", "current", language);
}

QString SettingsManager::loadLanguage()
{
    return loadValue("Language", "current").toString();
}


