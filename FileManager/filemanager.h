#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QTabWidget>
#include <QString>
#include <QWidget>

// The FileManager class handles saving and loading tab names
// to and from a .builder file for a QTabWidget, using
// standard file dialogs for user interaction.

class FileManager : public QObject {
    Q_OBJECT

public:
    explicit FileManager(QTabWidget* tabWidget, QObject* parent = nullptr);

    // Open with double clicking on .builder file
    bool loadFromPath(const QString& path);

    // Open a "Save As" dialog to choose a .builder file and save
    bool saveWithDialog();
    // Open an "Open" dialog to choose a .builder file and load
    bool loadWithDialog();

    // Direct save/load by filename
    bool save();
    bool load();

    bool isOpenedTableModified();
    bool isFileDataLoaded();

    QString getFileName();


signals:
    void sendExpressionFromFile(QString, bool);
    void sendNewTitle(QString);
    void changeActionStatus(bool);
    void clearAllTabs();
public slots:
    void changeDataLoaded(bool value);

private:
    QTabWidget* m_tabWidget;
    QStringList _lastSavedTabs;
    QStringList getCurrentTabs();

    QString currentFilePath = "";
    QString fileName;


    bool dataLoaded = false;

};

#endif
