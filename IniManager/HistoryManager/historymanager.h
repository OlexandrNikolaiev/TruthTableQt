#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include "../inimanager.h"
#include <QStringList>

class HistoryManager : public IniManager {
public:
    HistoryManager(const QString& fileName = "history.ini");

    void saveHistory(const QStringList& history);
    QStringList loadHistory();
};

#endif // HISTORYMANAGER_H
