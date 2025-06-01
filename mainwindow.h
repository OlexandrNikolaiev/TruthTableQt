#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QVector>
#include <QActionGroup>
#include "truthtablebuilder.h"
#include "IniManager/SettingsManager/settingsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_auxiliaryButton_clicked();
    void onMenuActionTriggered(bool checked);

    void on_buildButton_clicked();
    void on_clearButton_clicked();
    void on_truthTable_cellEntered(int row, int column);

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<TruthTableBuilder> tableBuilder;
    int varCount;
    QVector<QVector<int>> childDeps;
    void clearRowHighlight(int row);

    SettingsManager* settings;
    QActionGroup* colorGroup;
    QColor currentCellHoverColor;

    void loadSettings();
};

#endif // MAINWINDOW_H
