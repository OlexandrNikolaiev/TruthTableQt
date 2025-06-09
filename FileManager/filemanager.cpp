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
                                                    tr("Save Tabs"),
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
                                                    tr("Open Tabs"),
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
    QFile file(currentFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for reading:" << currentFilePath;
        return false;
    }

    emit sendNewTitle("| Відкрито: " + getFileName(currentFilePath));
    emit changeActionStatus(true);

    // Clear existing tabs
    _lastSavedTabs.clear();

    //emit clearAllTabs();

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
    return dataLoaded;
}

void FileManager::changeDataLoaded(bool value)
{
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

QString FileManager::getFileName(QString filePath)
{
    QFileInfo fileInfo(currentFilePath);
    return fileInfo.fileName();
}
