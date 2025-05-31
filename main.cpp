#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QTableWidget>
#include <QHeaderView>
#include "TruthTableBuilder.h"
#include "mainwindow.h"

#include <QFontDatabase>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString openSansSemiBols = ":/fonts/fonts/OpenSans-SemiBold.ttf";
    QFontDatabase::addApplicationFont(openSansSemiBols);

    MainWindow w;
    //w.setWindowIcon(QIcon("qrc:/icons/icons/icon3.ico"));
    w.show();
    return a.exec();
}

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     QWidget window;
//     window.setWindowTitle("Генератор таблиць істинності");
//     window.resize(1000, 600);

//     auto *mainLayout = new QVBoxLayout(&window);

//     auto *topLayout = new QHBoxLayout();
//     auto *input = new QLineEdit();
//     input->setPlaceholderText("Введіть логічний вираз, наприклад: a+b+c+d*e");
//     topLayout->addWidget(input);

//     topLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

//     auto *clearBtn = new QPushButton("Очистити");
//     auto *buildBtn = new QPushButton("Побудувати");
//     topLayout->addWidget(clearBtn);
//     topLayout->addWidget(buildBtn);

//     mainLayout->addLayout(topLayout);

//     // Таблиця
//     auto *table = new QTableWidget();
//     mainLayout->addWidget(table);

//     TruthTableBuilder builder;

//     auto rebuild = [&]() {
//         std::string expr = input->text().toStdString();
//         builder.setExpression(expr);
//         if (!builder.build()) {
//             // QMessageBox
//             return;
//         }
//         int n = builder.varCount();
//         int m = builder.subexprCount();
//         int rows = builder.rowCount();
//         int cols = n + m;

//         table->clear();
//         table->setRowCount(rows);
//         table->setColumnCount(cols);

//         QStringList headers;
//         for (char v : builder.getVarList())
//             headers << QString(v);
//         for (const auto &s : builder.getSubexprList())
//             headers << QString::fromStdString(s);
//         table->setHorizontalHeaderLabels(headers);

//         for (int i = 0; i < rows; ++i) {
//             for (int j = 0; j < cols; ++j) {
//                 bool val = builder.value(i, j);
//                 table->setItem(i, j, new QTableWidgetItem(val ? "1" : "0"));
//             }
//         }

//         auto *header = table->horizontalHeader();
//         for (int j = 0; j < n; ++j)
//             header->setSectionResizeMode(j, QHeaderView::ResizeToContents);
//         for (int j = n; j < cols; ++j)
//             header->setSectionResizeMode(j, QHeaderView::Stretch);

//         table->resizeColumnsToContents(); // обновить
//     };

//     QObject::connect(buildBtn, &QPushButton::clicked, rebuild);
//     QObject::connect(input, &QLineEdit::returnPressed, rebuild);
//     QObject::connect(clearBtn, &QPushButton::clicked, [&]() {
//         input->clear();
//         table->clear();
//         table->setRowCount(0);
//     });

//     window.show();
//     return app.exec();
// }
