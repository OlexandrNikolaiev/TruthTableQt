#include "historymanager.h"

HistoryManager::HistoryManager(const QString& fileName)
    : IniManager(fileName)
{
}

void HistoryManager::saveHistory(const QStringList& history)
{
    settings.beginGroup("History");
    settings.remove(""); // Очищаємо стару історію
    for (int i = 0; i < history.size(); ++i) {
        settings.setValue(QString("expression%1").arg(i), history[i]);
    }
    settings.endGroup();
    settings.sync();
}

QStringList HistoryManager::loadHistory()
{
    QStringList history;
    settings.beginGroup("History");
    int i = 0;
    while (settings.contains(QString("expression%1").arg(i))) {
        history.append(settings.value(QString("expression%1").arg(i)).toString());
        ++i;
    }
    settings.endGroup();
    return history;
}
