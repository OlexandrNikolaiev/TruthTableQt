#include "FileManager.h"
#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QDebug>
#include <QFileDialog>

FileManager::FileManager(QTabWidget* tabWidget, QObject* parent)
    : QObject(parent), m_tabWidget(tabWidget)
{
}

bool FileManager::saveWithDialog()
{
    QWidget* parent = m_tabWidget->parentWidget();
    QString filter = "Builder Files (*.builder)";
    currentFilePath = QFileDialog::getSaveFileName(parent,
                                                    tr("Зберегти файл"),
                                                    QString(), filter);
    if (currentFilePath.isEmpty())
        return false;
    // Ensure extension
    if (!currentFilePath.endsWith(".builder", Qt::CaseInsensitive)) {
        currentFilePath += ".builder";
    }

    return save();
}

bool FileManager::loadWithDialog()
{
    QWidget* parent = m_tabWidget->parentWidget();
    QString filter = "Builder Files (*.builder)";
    currentFilePath = QFileDialog::getOpenFileName(parent,
                                                    tr("Відкрити файл"),
                                                    QString(), filter);


    if (currentFilePath.isEmpty())
        return false;


    return load();
}

bool FileManager::save()
{
    _lastSavedTabs.clear();
    QFile file(currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << currentFilePath;
        return false;
    }

    QTextStream out(&file);
    int count = m_tabWidget->count();
    for (int i = 0; i < count; ++i) {
        const QString title = m_tabWidget->tabText(i);
        _lastSavedTabs << title;
        out << title;
        if (i < count - 1)
            out << '\n';
    }
    qDebug()<<_lastSavedTabs;

    file.close();
    return true;
}

bool FileManager::load()
{

    emit clearAllTabs();

    QFile file(currentFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for reading:" << currentFilePath;
        return false;
    }

    emit sendNewTitle(tr("| Відкрито: ") + getFileName());
    emit changeActionStatus(true);

    // Clear existing tabs
    _lastSavedTabs.clear();


    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString title = in.readLine();
        _lastSavedTabs << title;
        emit sendExpressionFromFile(title);
        // QWidget* newTab = new QWidget();
        // m_tabWidget->addTab(newTab, title);
        qDebug()<<title;
    }

    changeDataLoaded(true);
    file.close();
    return true;
}

bool FileManager::isOpenedTableModified()
{
    qDebug()<<_lastSavedTabs;
    qDebug()<<getCurrentTabs();
    if (_lastSavedTabs==getCurrentTabs()) {
        return false;
    }
    return true;
}

bool FileManager::isFileDataLoaded()
{
    qDebug()<<"value = "<<dataLoaded;
    return dataLoaded;
}

void FileManager::changeDataLoaded(bool value)
{
    qDebug()<<"value = "<<value;
    dataLoaded = value;
}


QStringList FileManager::getCurrentTabs()
{
    QStringList list;
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        list << m_tabWidget->tabText(i);
    }
    return list;
}

QString FileManager::getFileName()
{
    QFileInfo fileInfo(currentFilePath);
    return fileInfo.fileName();
}
