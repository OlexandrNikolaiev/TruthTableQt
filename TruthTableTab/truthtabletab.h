#ifndef TRUTHTABLETAB_H
#define TRUTHTABLETAB_H

#include <QWidget>
#include <QMessageBox>
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
    int getExpersionType() { return expressionType; }
    QString extractTopLevelOperator(const QString& header);
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void build(QString expression);
    void changeCellHoverColor(QColor color);
    void clear();

private slots:
    void on_truthTable_cellEntered(int row, int column);

signals:
    void sendExpressionTypeSignal(int);
    void statusMessageRequested(QString);


private:
    Ui::Tab *ui;
    TruthTableBuilder* tableBuilder;

    int varCount;
    QVector<QVector<int>> childDeps;

    QColor currentCellHoverColor;

    void clearRowHighlight(int row);

    int expressionType;
    QString currentHoveredOperator; //unused if commented


    void determineExpressionType();

    const QMap<QChar, QString> operatorNames = {
        {QChar(0x00AC), "Заперечення"},
        {QChar(0x2227), "Кон'юнкція"},
        {QChar(0x2228), "Диз'юнкція"},
        {QChar(0x21D2), "Імплікація"},
        {QChar(0x21D4), "Еквівалентність"}
    };

};

#endif // TRUTHTABLETAB_H
