#include "truthtabletab.h"
#include "ui_truthtabletab.h"

Tab::Tab(QWidget *parent, QString expression, QColor cellHoverColor)
    : QWidget(parent),
    ui(new Ui::Tab),
    varCount(0),
    currentCellHoverColor(cellHoverColor)
{
    ui->setupUi(this);
    tableBuilder = TruthTableBuilder::getInstance();

    ui->truthTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->truthTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->truthTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(ui->truthTable->horizontalHeader(), &QHeaderView::sectionResized,
            ui->truthTable, [this](int, int, int) {
                ui->truthTable->resizeRowsToContents();
            });
    connect(ui->truthTable,      &QTableWidget::cellEntered,  this, &Tab::on_truthTable_cellEntered);

    ui->truthTable->installEventFilter(this);
}

Tab::~Tab()
{
    delete ui;
}

QString Tab::extractTopLevelOperator(const QString& header) {
    if (header.startsWith("¬")) {
        return tr("заперечення");
    }
    // Якщо підвираз у дужках, наприклад "(A ∧ B)", витягуємо бінарну операцію
    else if (header.startsWith("(") && header.endsWith(")")) {
        QString inner = header.mid(1, header.size() - 2); // Видаляємо зовнішні дужки
        int level = 0;
        for (int i = 0; i < inner.size(); ++i) {
            QChar ch = inner[i];
            if (ch == '(') level++;
            else if (ch == ')') level--;
            else if (level == 0) {
                if (ch == QChar(0x2227)) return tr("кон'юнкція");
                if (ch == QChar(0x2228)) return tr("диз'юнкція");
                if (ch == QChar(0x21D2)) return tr("імплікація");
                if (ch == QChar(0x21D4)) return tr("еквіваленція");
            }
        }
    }
    return "";
}

bool Tab::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->truthTable && event->type() == QEvent::Leave) {
        emit statusMessageRequested("");
    }
    return QWidget::eventFilter(obj, event);
}

void Tab::setExecutionTime(QString time)
{
    executionTime = time;
}

void Tab::build(QString expression)
{
    tableBuilder->setExpression(expression);
    if (!tableBuilder->build()) {
        QMessageBox::critical(this, "Помилка", "Не вдалося побудувати таблицю істинності.");
        return;
    }

    varCount = tableBuilder->varCount();
    childDeps = tableBuilder->getChildDependencies();

    int subexprCount = tableBuilder->subexprCount();
    int rows = tableBuilder->rowCount();
    cols = varCount + subexprCount;

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

    restretchTable();

    determineExpressionType();
}

void Tab::restretchTable()
{
    if (!isStretched) {
        auto *hdr = ui->truthTable->horizontalHeader();
        for (int j = 0; j < varCount; ++j) {
            hdr->setSectionResizeMode(j, QHeaderView::ResizeToContents);
            ui->truthTable->horizontalHeaderItem(j)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }
        for (int j = varCount; j < cols; ++j) {
            hdr->setSectionResizeMode(j, QHeaderView::ResizeToContents);
            ui->truthTable->horizontalHeaderItem(j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        ui->truthTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->truthTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        isStretched = true;
    }  else {
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
        isStretched = false;
    }
}

void Tab::changeCellHoverColor(QColor color)
{
    qDebug()<<"changed to "<<color;
    currentCellHoverColor = color;
}

void Tab::clear()
{
    ui->truthTable->clearContents();
    childDeps.clear();
}

void Tab::clearRowHighlight(int row)
{
    int cols = ui->truthTable->columnCount();
    for (int j = 0; j < cols; ++j) {
        if (auto item = ui->truthTable->item(row, j))
        {

            item->setBackground(Qt::white);
        }
    }
}



void Tab::determineExpressionType()
{
    int rowCount = ui->truthTable->rowCount();
    int lastColumn = ui->truthTable->columnCount() - 1;

    bool allTrue = true;
    bool allFalse = true;

    for (int row = 0; row < rowCount; ++row) {
        QTableWidgetItem* item = ui->truthTable->item(row, lastColumn);
        if (!item) {
            continue;
        }

        QString value = item->text().trimmed();

        if (value != "1") {
            allTrue = false;
        }
        if (value != "0") {
            allFalse = false;
        }
    }

    if (allTrue) {
        expressionType = 0;
        emit sendExpressionTypeSignal(expressionType);
    } else if (allFalse) {
        expressionType = 1;
        emit sendExpressionTypeSignal(expressionType);
    } else {
        expressionType = 2;
        emit sendExpressionTypeSignal(expressionType);
    }
}




void Tab::on_truthTable_cellEntered(int row, int column) {
    int rows = ui->truthTable->rowCount();
    for (int i = 0; i < rows; i++) {
        clearRowHighlight(i);
    }
    if (column >= varCount) {
        int idx = column - varCount;
        for (int childCol : childDeps[idx]) {
            if (auto item = ui->truthTable->item(row, childCol))
                item->setBackground(QBrush(QColor(currentCellHoverColor)));
        }
    }

    if (column < varCount) {
        // змінні
        QString var = ui->truthTable->horizontalHeaderItem(column)->text();
        emit statusMessageRequested(tr("Змінна: ") + var);
    } else {
        // підвирази
        QString header = ui->truthTable->horizontalHeaderItem(column)->text();
        QString op = extractTopLevelOperator(header);
        if (!op.isEmpty()) {
            emit statusMessageRequested(tr("Операція: ") + op);
        } else {
            emit statusMessageRequested(tr("Підвираз: ") + header); // nikogda ne rabotaet
        }

    }
}



