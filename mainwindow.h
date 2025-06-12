#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QVector>
#include <QActionGroup>
#include <QScopedPointer>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>
#include <QTranslator>

#include "TruthTableTab/truthtabletab.h"
#include "IniManager/SettingsManager/settingsmanager.h"
#include "FileManager/filemanager.h"
#include "ExcelExporter/excelexporter.h"
#include "IniManager/HistoryManager/historymanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //public for testing
    QString validateExpression(const QString& expr) const;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTabDoubleClicked(int index);

    void onHistoryEntrySelected(const QString& expression);

    void on_auxiliaryButton_clicked();
    void onMenuActionTriggered(bool checked);
    void onLanguageActionTriggered(bool checked);

    void on_buildButton_clicked();

    void closeTab(int tabNumber);
    void onTabChanged(int index);

    void closeEvent(QCloseEvent *event);

    void setExpressionType(int type);

    void changeCurrentOperationText(QString text);
    void setStatusBarText(QString text);

    void changeWindowTitle(QString newTitle);
    void changeSaveAction(bool value);

    void closeOpenedFile();

    void exportToExcel();
signals:
    void changeCellHoverColorSignal(QColor);
    void changeDataLoaded(bool);
private:
    Ui::MainWindow *ui;
    HistoryManager* historyManager;

    QTranslator* translator;
    bool isUkrainian;
    void switchLanguage();
    void changeEvent(QEvent *event);
    //  void retranslateUi();

    QLabel* executionTimeLabel;

    SettingsManager* settings;

    QActionGroup* languageGroup;

    QColor currentCellHoverColor;
    QActionGroup* colorGroup;
    Tab* _tab;

    FileManager* fileManager;

    void loadSettings();

    QVector<QChar> extractVariables(const QString& expr) const;
    int findTabIndexByName(QTabWidget *tabWidget, const QString &tabName);

    void build(QString expression, bool addToHistory);


};

#endif // MAINWINDOW_H
