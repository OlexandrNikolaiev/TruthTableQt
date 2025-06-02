#ifndef TRUTHTABLETAB_H
#define TRUTHTABLETAB_H

#include <QWidget>
#include <QMessageBox>
#include "../truthtablebuilder.h"

namespace Ui {
class Tab;
}

class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = nullptr, QString expression = "", QColor cellHoverColor = "#00FFFFFF");
    ~Tab();

public slots:
    void build(QString expression);
    void changeCellHoverColor(QColor color);
    void clear();

private slots:
    void on_truthTable_cellEntered(int row, int column);


private:
    Ui::Tab *ui;
    TruthTableBuilder* tableBuilder;

    int varCount;
    QVector<QVector<int>> childDeps;

    QColor currentCellHoverColor;

    void clearRowHighlight(int row);



};

#endif // TRUTHTABLETAB_H
