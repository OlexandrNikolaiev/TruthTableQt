#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <QTableWidget>
#include <QString>

class ExcelExporter {
public:
    explicit ExcelExporter();

    static bool exportToExcel(QTableWidget* table,
                              const QString& filePath,
                              const QString& title,
                              QString* error = nullptr);

private:
    QTableWidget* table;
};

#endif
