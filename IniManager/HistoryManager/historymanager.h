#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QObject>
#include <QMenu>
#include "../SettingsManager/settingsmanager.h"

class HistoryManager : public QObject
{
    Q_OBJECT
public:
    HistoryManager(QMenu* menu, SettingsManager* settings, QObject* parent = nullptr);
    void addEntry(const QString& expression);
    void loadHistory();

signals:
    void entrySelected(const QString& expression);

private:
    void saveHistory();
    void updateMenu();

    QMenu* _menu;
    SettingsManager* _settings;
    QStringList _history;
};

#endif // HISTORYMANAGER_H
