#include "IniManager.h"

IniManager::IniManager(const QString& fileName)
    : settings(QCoreApplication::applicationDirPath() + "/" + fileName, QSettings::IniFormat)
{
    qDebug()<<"here";
}

void IniManager::saveValue(const QString& group, const QString& key, const QVariant& value)
{
    settings.beginGroup(group);
    settings.setValue(key, value);
    settings.endGroup();
    settings.sync();
}

QVariant IniManager::loadValue(const QString& group, const QString& key, const QVariant& defaultValue)
{
    settings.beginGroup(group);
    QVariant value = settings.value(key, defaultValue);
    settings.endGroup();
    return value;
}
