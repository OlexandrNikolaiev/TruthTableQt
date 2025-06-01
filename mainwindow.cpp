// MainWindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tableBuilder(std::make_unique<TruthTableBuilder>())
    , varCount(0)
{
    ui->setupUi(this);
    this->setWindowTitle("Генератор таблиць істинності");
    ui->inputLineEdit->setPlaceholderText("Введіть логічний вираз...");

    connect(ui->inputLineEdit, &QLineEdit::returnPressed,
            this, &MainWindow::on_buildButton_clicked);
    connect(ui->calculateButton, &QPushButton::clicked,
            this, &MainWindow::on_buildButton_clicked);
    connect(ui->clearButton, &QPushButton::clicked,
            this, &MainWindow::on_clearButton_clicked);

    ui->truthTable->setMouseTracking(true);
    ui->truthTable->viewport()->setMouseTracking(true);
    connect(ui->truthTable, &QTableWidget::cellEntered,
            this, &MainWindow::on_truthTable_cellEntered);

    ui->horizontalLayout_2->setStretch(0,0);

    ui->truthTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->truthTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->truthTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->truthTable->horizontalHeader(), &QHeaderView::sectionResized,
            ui->truthTable, [this](int, int, int) {
                ui->truthTable->resizeRowsToContents();
            });

    ui->statusbar->setVisible(true);
    ui->statusbar->showMessage("123");

    ui->action_2->setChecked(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buildButton_clicked()
{
    QString expr = ui->inputLineEdit->text().trimmed();
    // if (expr.isEmpty()) {
    //     QMessageBox::warning(this, "Помилка", "Ви не ввели логічний вираз!");
    //     return;
    // }

    // Перевірка виразу
    std::string error = tableBuilder->validateExpression(expr.toStdString());
    if (!error.empty()) {
        QMessageBox::warning(this, "Помилка", QString::fromStdString(error));
        return;
    }

    varCount = tableBuilder->varCount();
    childDeps = tableBuilder->getChildDependencies();

    int subexprCount = tableBuilder->subexprCount();
    int rows = tableBuilder->rowCount();
    int cols = varCount + subexprCount;

    ui->truthTable->clear();
    ui->truthTable->setRowCount(rows);
    ui->truthTable->setColumnCount(cols);

    QStringList headers;
    for (char var : tableBuilder->getVarList())
        headers << QString(var);

    for (const auto &sub : tableBuilder->getSubexprList()) {
        QString sym = QString::fromStdString(sub);
        sym.replace("!", "¬");
        sym.replace("*", "∧");
        sym.replace("+", "∨");
        sym.replace(">", "⇒");
        sym.replace("=", "⇔");
        headers << sym;
    }
    ui->truthTable->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            bool val = tableBuilder->value(i, j);
            auto* item = new QTableWidgetItem(val ? "1" : "0");
            if (j < varCount) {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            } else {
                item->setTextAlignment(Qt::AlignCenter);
            }
            ui->truthTable->setItem(i, j, item);
        }
    }


    auto *hdr = ui->truthTable->horizontalHeader();

    // vars
    for (int j = 0; j < varCount; ++j) {
        hdr->setSectionResizeMode(j, QHeaderView::ResizeToContents);
        ui->truthTable->horizontalHeaderItem(j)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    //se
    for (int j = varCount; j < cols; ++j) {
        hdr->setSectionResizeMode(j, QHeaderView::Stretch);
        ui->truthTable->horizontalHeaderItem(j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }


    ui->truthTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);



}


void MainWindow::on_clearButton_clicked()
{
    ui->inputLineEdit->clear();
    ui->inputLineEdit->setFocus();
    ui->truthTable->clearContents();

    childDeps.clear();
}

void MainWindow::clearRowHighlight(int row)
{
    int cols = ui->truthTable->columnCount();
    for (int j = 0; j < cols; ++j) {
        if (auto item = ui->truthTable->item(row, j))
            item->setBackground(Qt::white);
    }
}

void MainWindow::on_truthTable_cellEntered(int row, int column)
{
    int rows = ui->truthTable->rowCount();
    for (int i = 0; i<rows; i++)
    {
        clearRowHighlight(i);

    }
    // Завжди очищаємо всі підсвічування у цій рядку
    //clearRowHighlight(row);

    // Якщо наведено на підвираз (колонка >= кількість змінних) —
    // підсвічуємо лише залежні від нього стовпці в цій же рядку
    if (column >= varCount) {
        int idx = column - varCount;
        for (int childCol : childDeps[idx]) {
            if (auto item = ui->truthTable->item(row, childCol))
                item->setBackground(Qt::yellow);
        }
    }
}





void MainWindow::on_action_2_toggled(bool arg1)
{
    ui->frame->setVisible(arg1);
    ui->centralwidget->layout()->invalidate();

}

