// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>
#include "truthtablebuilder.h"

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
    void on_buildButton_clicked();
    void on_clearButton_clicked();
    void on_truthTable_cellEntered(int row, int column);


    void on_action_2_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<TruthTableBuilder> tableBuilder;
    int varCount;
    std::vector<std::vector<int>> childDeps;
    void clearRowHighlight(int row);
};

#endif // MAINWINDOW_H
