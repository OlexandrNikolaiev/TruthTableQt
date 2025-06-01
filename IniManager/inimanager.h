#ifndef INIMANAGER_H
#define INIMANAGER_H

#include <QSettings>
#include <QCoreApplication>

class IniManager {
public:
    IniManager(const QString& fileName);
    virtual ~IniManager() = default;

    void saveValue(const QString& group, const QString& key, const QVariant& value);
    QVariant loadValue(const QString& group, const QString& key, const QVariant& defaultValue = QVariant());

protected:
    QSettings settings;
};

#endif // INIMANAGER_H
