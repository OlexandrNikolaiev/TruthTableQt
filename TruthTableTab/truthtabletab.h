#ifndef TRUTHTABLETAB_H
#define TRUTHTABLETAB_H

#include <QWidget>
#include <QMessageBox>
#include <QDateTime>
#include "../TruthTableBuilder/truthtablebuilder.h"

namespace Ui {
class Tab;
}

class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = nullptr, QString expression = "", QColor cellHoverColor = "#00FFFFFF");
    ~Tab();
    int getExpressionType() { return expressionType; }
    QString getExectuionTime() { return executionTime; }
    QString extractTopLevelOperator(const QString& header);
    bool eventFilter(QObject *obj, QEvent *event);
    void setExecutionTime(QString time);

    void restretchTable();


public slots:
    void build(QString expression);
    void changeCellHoverColor(QColor color);
    void clear();

private slots:
    void on_truthTable_cellEntered(int row, int column);

signals:
    void sendExpressionTypeSignal(int);
    void statusMessageRequested(QString);
    void sendExecutionTime(QString);


private:


    Ui::Tab *ui;
    TruthTableBuilder* tableBuilder;

    int cols;
    int varCount;
    QVector<QVector<int>> childDeps;

    QColor currentCellHoverColor;

    void clearRowHighlight(int row);

    int expressionType;
    QString currentHoveredOperator; //unused if commented


    void determineExpressionType();

    bool isStretched = true;

    QString executionTime;

};

#endif // TRUTHTABLETAB_H
