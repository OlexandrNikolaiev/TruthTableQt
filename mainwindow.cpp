#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <qactiongroup.h>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tableBuilder(std::make_unique<TruthTableBuilder>())
    , varCount(0)
{
    ui->setupUi(this);
    settings = new SettingsManager("settings.ini");



    this->setWindowTitle("Генератор таблиць істинності");
    ui->inputLineEdit->setPlaceholderText("Введіть логічний вираз...");

    connect(ui->inputLineEdit,   &QLineEdit::returnPressed,   this, &MainWindow::on_buildButton_clicked);
    connect(ui->calculateButton, &QPushButton::clicked,       this, &MainWindow::on_buildButton_clicked);
    connect(ui->clearButton,     &QPushButton::clicked,       this, &MainWindow::on_clearButton_clicked);

    connect(ui->truthTable,      &QTableWidget::cellEntered,  this, &MainWindow::on_truthTable_cellEntered);

    ui->horizontalLayout_2->setStretch(0,0);

    ui->truthTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->truthTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->truthTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->truthTable->horizontalHeader(), &QHeaderView::sectionResized,
            ui->truthTable, [this](int, int, int) {
                ui->truthTable->resizeRowsToContents();
            });

    //ui->frame_5->hide();
    ui->statusbar->setVisible(true);
    ui->statusbar->showMessage("123");

    //ui->truthTable->hide();


    connect(ui->negationButton,      &QPushButton::clicked, this, &MainWindow::on_auxiliaryButton_clicked);
    connect(ui->conjunctionButton,   &QPushButton::clicked, this, &MainWindow::on_auxiliaryButton_clicked);
    connect(ui->disjunctionButton,   &QPushButton::clicked, this, &MainWindow::on_auxiliaryButton_clicked);
    connect(ui->implicationButton,   &QPushButton::clicked, this, &MainWindow::on_auxiliaryButton_clicked);
    connect(ui->biconditionalButton, &QPushButton::clicked, this, &MainWindow::on_auxiliaryButton_clicked);

    ui->menubar->setStyleSheet(
        "QMenuBar {"
        "    background-color: rgb(36, 47, 61);"
        "    color: white;"
        "}"
        "QMenuBar::item {"
        "    background-color: rgb(36, 47, 61);"
        "    color: white;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: rgb(37, 48, 62);"
        "}"
        "QMenu {"
        "    background-color: rgb(40, 50, 64);"
        "    color: white;"
        "}"
        "QMenu::item:selected {"
        "    background-color: rgb(40, 50, 64);"
        "}"
        );




    connect(ui->action_showAuxButtons, &QAction::toggled, this, &MainWindow::onMenuActionTriggered);
    connect(ui->action_green, &QAction::toggled, this, &MainWindow::onMenuActionTriggered);
    connect(ui->action_grey, &QAction::toggled, this, &MainWindow::onMenuActionTriggered);
    connect(ui->action_yellow, &QAction::toggled, this, &MainWindow::onMenuActionTriggered);

    colorGroup = new QActionGroup(this);
    colorGroup->addAction(ui->action_green);
    colorGroup->addAction(ui->action_grey);
    colorGroup->addAction(ui->action_yellow);
    colorGroup->setExclusive(false);

    ui->inputLineEdit->setFocus();
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_auxiliaryButton_clicked()
{
    QObject* senderObj = sender();
    if (qobject_cast<QPushButton*>(senderObj) == ui->negationButton) {
        ui->inputLineEdit->insert("¬");
    } else if (qobject_cast<QPushButton*>(senderObj) == ui->conjunctionButton) {
        ui->inputLineEdit->insert("∧");
    } else if (qobject_cast<QPushButton*>(senderObj) == ui->disjunctionButton) {
        ui->inputLineEdit->insert("∨");
    } else if (qobject_cast<QPushButton*>(senderObj) == ui->implicationButton) {
        ui->inputLineEdit->insert("⇒");
    } else if (qobject_cast<QPushButton*>(senderObj) == ui->biconditionalButton) {
        ui->inputLineEdit->insert("⇔");
    } else {
        qDebug() << "Unknown button clicked!";
    }
}

void MainWindow::onMenuActionTriggered(bool checked) // peredelat' etot pizdets
{
    QAction *senderAction = qobject_cast<QAction*>(sender());
    if (!senderAction) return;


    if (senderAction == ui->action_showAuxButtons) {
        if (checked) {
            qDebug()<<"Aux Checked";
            ui->frame_5->show();
        } else {
            qDebug()<<"Aux not Checked";
            ui->frame_5->hide();
        }
        settings->saveShowAuxButtons(ui->action_showAuxButtons->isChecked());
        return;
    }

    if (senderAction == ui->action_green) {
        if (checked) {
            qDebug()<<"From green checked";
            currentCellHoverColor = QColor("#22b14c");
            ui->action_grey->setChecked(false);
            ui->action_yellow->setChecked(false);
            settings->saveCellHoverColor(currentCellHoverColor);
        }
    } else if (senderAction == ui->action_grey) {
        if (checked) {
            qDebug()<<"From grey checked";
            currentCellHoverColor = QColor("#c3c3c3");
            ui->action_green->setChecked(false);
            ui->action_yellow->setChecked(false);
            settings->saveCellHoverColor(currentCellHoverColor);
        }
    } else if (senderAction == ui->action_yellow) {
        if (checked) {
            qDebug()<<"From yellow checked";
            currentCellHoverColor = QColor("#ffff00");
            ui->action_green->setChecked(false);
            ui->action_grey->setChecked(false);
            settings->saveCellHoverColor(currentCellHoverColor);
        }
    } else {
        qDebug()<<"Nothing checked";
        currentCellHoverColor = QColor("#00FFFFFF");
        ui->action_green->setChecked(false);
        ui->action_grey->setChecked(false);
        ui->action_yellow->setChecked(false);
        settings->saveCellHoverColor(currentCellHoverColor);
    }
}

void MainWindow::on_buildButton_clicked()
{
    QString expr = ui->inputLineEdit->text().trimmed();
    if (expr.isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Ви не ввели логічний вираз!");
        return;
    }

    QString error = tableBuilder->validateExpression(expr);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Помилка", error);
        return;
    }

    tableBuilder->setExpression(expr);
    if (!tableBuilder->build()) {
        QMessageBox::critical(this, "Помилка", "Не вдалося побудувати таблицю істинності.");
        return;
    }

    varCount = tableBuilder->varCount();
    childDeps = tableBuilder->getChildDependencies();

    int subexprCount = tableBuilder->subexprCount();
    int rows = tableBuilder->rowCount();
    int cols = varCount + subexprCount;

    ui->truthTable->show();

    ui->truthTable->clear();
    ui->truthTable->setRowCount(rows);
    ui->truthTable->setColumnCount(cols);

    QStringList headers;
    for (QChar var : tableBuilder->getVarList())
        headers << QString(var);

    for (const QString &sub : tableBuilder->getSubexprList()) {
        QString sym = sub;
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

    for (int j = 0; j < varCount; ++j) {
        hdr->setSectionResizeMode(j, QHeaderView::ResizeToContents);
        ui->truthTable->horizontalHeaderItem(j)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

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

void MainWindow::loadSettings() //todo aux button
{
    QByteArray geometry = settings->loadWindowGeometry(); //todo if previous res bigger than current one
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    } else {
        resize(800, 600);
        move(100, 100);
    }

    bool showAuxButtons = settings->loadShowAuxButtons();
    ui->action_showAuxButtons->setChecked(showAuxButtons);
    if (showAuxButtons) {
        ui->frame_5->show();
    } else {
        ui->frame_5->hide();
    }

    currentCellHoverColor = settings->loadCellHoverColor();
    qDebug()<<"current = "<<currentCellHoverColor;
    if (currentCellHoverColor == QColor("#22b14c")) {
        qDebug()<<"green";
        ui->action_green->setChecked(true);
    } else if (currentCellHoverColor == QColor("#c3c3c3")) {
        qDebug()<<"grey";
        ui->action_grey->setChecked(true);
    } else if (currentCellHoverColor == QColor("#ffff00")) {
        qDebug()<<"yellow";
        ui->action_yellow->setChecked(true);
    } else {
        qDebug()<<"#00FFFFFF";
        ui->action_green->setChecked(false);
        ui->action_grey->setChecked(false);
        ui->action_yellow->setChecked(false);
    }



}

void MainWindow::on_truthTable_cellEntered(int row, int column)
{
    int rows = ui->truthTable->rowCount();
    for (int i = 0; i < rows; i++) {
        clearRowHighlight(i);
    }

    if (column >= varCount) {
        int idx = column - varCount;
        for (int childCol : childDeps[idx]) {
            if (auto item = ui->truthTable->item(row, childCol))
                item->setBackground(currentCellHoverColor);
        }
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    settings->saveWindowGeometry(saveGeometry());
    QMainWindow::closeEvent(event);
}
