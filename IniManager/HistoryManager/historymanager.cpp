#include "historymanager.h"
#include <QDateTime>
#include <QAction>

HistoryManager::HistoryManager(QMenu* menu, SettingsManager* settings, QObject* parent)
    : QObject(parent), _menu(menu), _settings(settings)
{
}

void HistoryManager::addEntry(const QString& expression)
{
    QString timestamp = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm");
    QString entry = "[" + timestamp + "] " + expression;
    _history.prepend(entry);
    if (_history.size() > 20) {
        _history.removeLast();
    }
    saveHistory();
    updateMenu();
}

void HistoryManager::loadHistory()
{
    _history = _settings->loadValue("History", "entries").toStringList();
    updateMenu();
}

void HistoryManager::saveHistory()
{
    _settings->saveValue("History", "entries", _history);
}

void HistoryManager::updateMenu()
{
    _menu->clear();
    QAction* emptyAction;
    if (_history.isEmpty()) {
        emptyAction = new QAction(tr("Історія порожня"), _menu);
        emptyAction->setEnabled(false);
        _menu->addAction(emptyAction);
    }
    else {
        for (const QString& entry : _history) {
            QAction* action = _menu->addAction(entry);
            connect(action, &QAction::triggered, this, [this, entry]() {
                int idx = entry.indexOf(']');
                if (idx != -1) {
                    QString expression = entry.mid(idx + 2).trimmed();
                    emit entrySelected(expression);
                }
            });
        }
    }


}
