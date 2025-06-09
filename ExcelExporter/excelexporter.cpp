#include "ExcelExporter.h"
#include <QAxObject>
#include <QDir>
#include <QTableWidgetItem>

ExcelExporter::ExcelExporter() {}

bool ExcelExporter::exportToExcel(QTableWidget* table,
                                  const QString& filePath,
                                  const QString& title,
                                  QString* error) {
    if (!table) {
        if (error) *error = "Таблиця не задана.";
        return false;
    }

    QAxObject* excel = new QAxObject("Excel.Application");
    if (excel->isNull()) {
        delete excel;
        if (error) *error = "Не вдалося запустити Excel (QAxObject).";
        return false;
    }

    excel->setProperty("Visible", false);
    QAxObject* workbooks = excel->querySubObject("Workbooks");
    QAxObject* workbook = workbooks->querySubObject("Add()");
    QAxObject* sheet = workbook->querySubObject("Worksheets(int)", 1);

    int rows = table->rowCount();
    int cols = table->columnCount();
    if (cols == 0) {
        if (error) *error = "Таблиця не містить стовпців.";
        excel->dynamicCall("Quit()");
        delete excel;
        return false;
    }

    QChar endCol = QChar('A' + cols - 1);
    QString endCell = QString("%1").arg(endCol) + "1";
    QString mergeRange = "A1:" + endCell;

    QAxObject* headerRange = sheet->querySubObject("Range(const QString&)", mergeRange);
    if (headerRange) {
        headerRange->dynamicCall("Merge()");
        headerRange->setProperty("Value", title);
        headerRange->setProperty("HorizontalAlignment", -4108);
        QAxObject* font = headerRange->querySubObject("Font");
        if (font) {
            font->setProperty("Bold", true);
            QAxObject* sampleCell = sheet->querySubObject("Cells(int,int)", 2, 1);
            QAxObject* sampleFont = sampleCell->querySubObject("Font");
            int defaultSize = sampleFont->property("Size").toInt();
            font->setProperty("Size", defaultSize + 2);
            delete sampleFont;
            delete sampleCell;
        }
        delete font;
        delete headerRange;
    }

    for (int col = 0; col < cols; ++col) {
        QString header = table->horizontalHeaderItem(col)->text();
        sheet->querySubObject("Cells(int,int)", 2, col + 1)->setProperty("Value", header);
    }

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QTableWidgetItem* item = table->item(row, col);
            QString text = item ? item->text() : "";
            sheet->querySubObject("Cells(int,int)", row + 3, col + 1)->setProperty("Value", text);
        }
    }

    QChar lastCol = QChar('A' + cols - 1);
    QString lastCell = QString("%1").arg(lastCol) + QString::number(rows + 2);
    QString tableRange = "A1:" + lastCell;

    QAxObject* range = sheet->querySubObject("Range(const QString&)", tableRange);
    if (range) {
        QAxObject* borders = range->querySubObject("Borders");
        if (borders) {
            borders->setProperty("LineStyle", 1);
            borders->setProperty("Weight", 2);
            delete borders;
        }
        delete range;
    }

    for (int col = 0; col < cols; ++col) {
        QAxObject* column = sheet->querySubObject("Columns(int)", col + 1);
        if (column) {
            column->dynamicCall("AutoFit()");
            delete column;
        }
    }

    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(filePath));
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    delete excel;

    return true;
}
