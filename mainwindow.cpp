#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <qactiongroup.h>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = new SettingsManager("settings.ini");



    this->setWindowTitle("Генератор таблиць істинності");
    ui->inputLineEdit->setPlaceholderText("Введіть логічний вираз...");

    connect(ui->inputLineEdit,   &QLineEdit::returnPressed,   this, &MainWindow::on_buildButton_clicked);
    connect(ui->calculateButton, &QPushButton::clicked,       this, &MainWindow::on_buildButton_clicked);
    connect(ui->backspaceButton, &QPushButton::clicked, this, [=]() {
        QString text = ui->inputLineEdit->text();
        if (!text.isEmpty()) {
            text.chop(1);
            ui->inputLineEdit->setText(text);
        }
    });
    //connect(ui->clearButton,     &QPushButton::clicked,       this, &MainWindow::on_clearButton_clicked);


    //ui->horizontalLayout_2->setStretch(0,0);


    //ui->frame_5->hide();
    ui->statusbar->setVisible(true);
    ui->statusbar->showMessage("123");

    //ui->truthTable->hide();

    auto makeShadow = [&]() {
        auto *shadow = new QGraphicsDropShadowEffect;
        shadow->setBlurRadius(5);
        shadow->setOffset(1, 1);
        shadow->setColor(QColor(0,0,0,150));
        return shadow;
    };

    ui->frame_2->setGraphicsEffect(makeShadow());
    ui->calculateButton->setGraphicsEffect(makeShadow());
    ui->backspaceButton->setGraphicsEffect(makeShadow());

    ui->negationButton->setGraphicsEffect(makeShadow());
    ui->conjunctionButton->setGraphicsEffect(makeShadow());
    ui->disjunctionButton->setGraphicsEffect(makeShadow());
    ui->implicationButton->setGraphicsEffect(makeShadow());
    ui->biconditionalButton->setGraphicsEffect(makeShadow());

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

    QTabBar *tabBar = ui->tabWidget->tabBar();
    QFont font = tabBar->font();
    font.setBold(true);
    font.setPointSize(12);
    tabBar->setFont(font);


    connect(ui->action_showAuxButtons, &QAction::toggled, this, &MainWindow::onMenuActionTriggered);
    connect(ui->action_green, &QAction::triggered, this, &MainWindow::onMenuActionTriggered);
    connect(ui->action_grey, &QAction::triggered, this, &MainWindow::onMenuActionTriggered);
    connect(ui->action_yellow, &QAction::triggered, this, &MainWindow::onMenuActionTriggered);

    colorGroup = new QActionGroup(this);
    colorGroup->addAction(ui->action_green);
    colorGroup->addAction(ui->action_grey);
    colorGroup->addAction(ui->action_yellow);
    colorGroup->setExclusive(false);

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

    ui->inputLineEdit->setFocus();

    ui->stackedWidget->setCurrentIndex(0);
    loadSettings();

    ui->welcomeLabel->setText(
        "<h1 style='text-align: center;'>Ласкаво просимо до Генератора таблиць істинності!</h1>"
        "<p style='text-align: center;'>Цей інструмент дозволяє швидко створювати таблиці істинності для логічних виразів. "
        "Вводьте вирази, будуйте таблиці та аналізуйте результати!</p>"

        "<h2 style='text-align: center;'>Як почати?</h2>"
        "<p style='text-align: center;'>"
        "1. &nbsp;Введіть логічний вираз у поле 'Введіть логічний вираз...' у верхній частині вікна.<br>"
        "2. &nbsp;Натисніть кнопку 'Побудувати' або клавішу Enter, щоб згенерувати таблицю істинності."
        "</p>"

        "<h2 style='text-align: center;'>Приклади виразів</h2>"
        "<p style='text-align: center;'>"
        "1. &nbsp;&nbsp;<span style='font-size: 20px; font-weight: bold;'>A ∧ B</span> — кон'юнкція A та B."
        "</p>"
        "<p style='text-align: center;'>"
        "2. &nbsp;&nbsp;<span style='font-size: 20px; font-weight: bold;'>¬(A ∨ B)</span> — заперечення диз'юнкції A і B."
        "</p>"
        "<p style='text-align: center;'>"
        "3. &nbsp;&nbsp;<span style='font-size: 20px; font-weight: bold;'>(A ⇒ B) ⇔ (¬A ∨ B)</span> — еквіваленція імплікації та її розкладення."
        "</p> <br>"

        "<h2 style='text-align: center;'>Бажаєте вдосконалити програму?</h2> "
        "<p style='text-align: center;'>"
        "<a href='https://github.com/OlexandrNikolaiev/TruthTableQt'>Відкритий код проєкту</a>"
        "</p>"
        );

    ui->welcomeImage->setTextFormat(Qt::RichText);          // Дозволяє HTML
    ui->welcomeImage->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->welcomeImage->setOpenExternalLinks(true);           // Відкривати посилання у браузері


    // ui->welcomeLabel_3->ins(
    //     "<h2>Приклади виразів:</h2>"

    //     );
    QPixmap pixmap("builder.png");
    if (pixmap.isNull()) {
        qDebug() << "Не удалось загрузить изображение!";
    } else {
        ui->welcomeImage->setPixmap(pixmap);
    }

    ui->welcomeImage->setPixmap(pixmap.scaled(ui->welcomeImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->welcomeImage->setScaledContents(true); // Если хочешь, чтобы QLabel сам масштабировал изображение


    ui->welcomeLabel->setWordWrap(true);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
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
    ui->inputLineEdit->setFocus();
}

void MainWindow::onMenuActionTriggered(bool checked) // peredelat' etot pizdets
{
    QAction *senderAction = qobject_cast<QAction*>(sender());
    qDebug()<<"1";
    if (!senderAction) return;
    qDebug()<<"2";


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
    qDebug()<<"3";

    if (senderAction == ui->action_green) {
        if (checked) {
            qDebug() << "From green checked";
            currentCellHoverColor = QColor("#22b14c");
        }
        else {
            qDebug() << "Green was unchecked → transparent";
            currentCellHoverColor = QColor(Qt::transparent);
        }
        // In both cases, uncheck the other two:
        ui->action_grey->setChecked(false);
        ui->action_yellow->setChecked(false);
        settings->saveCellHoverColor(currentCellHoverColor);
        emit changeCellHoverColorSignal(currentCellHoverColor);
    }
    else if (senderAction == ui->action_grey) {
        if (checked) {
            qDebug() << "From grey checked";
            currentCellHoverColor = QColor("#c3c3c3");
        }
        else {
            qDebug() << "Grey was unchecked → transparent";
            currentCellHoverColor = QColor(Qt::transparent);
        }
        ui->action_green->setChecked(false);
        ui->action_yellow->setChecked(false);
        settings->saveCellHoverColor(currentCellHoverColor);
        emit changeCellHoverColorSignal(currentCellHoverColor);
    }
    else if (senderAction == ui->action_yellow) {
        if (checked) {
            qDebug() << "From yellow checked";
            currentCellHoverColor = QColor("#ffff00");
        }
        else {
            qDebug() << "Yellow was unchecked → transparent";
            currentCellHoverColor = QColor(Qt::transparent);
        }
        ui->action_green->setChecked(false);
        ui->action_grey->setChecked(false);
        settings->saveCellHoverColor(currentCellHoverColor);
        emit changeCellHoverColorSignal(currentCellHoverColor);
    }
    else {
        qDebug() << "None of the color‐actions. Fallback to fully transparent.";
        currentCellHoverColor = QColor(Qt::transparent);
        ui->action_green->setChecked(false);
        ui->action_grey->setChecked(false);
        ui->action_yellow->setChecked(false);
        settings->saveCellHoverColor(currentCellHoverColor);
        emit changeCellHoverColorSignal(currentCellHoverColor);
    }


}

void MainWindow::on_buildButton_clicked()
{
    QString expression = ui->inputLineEdit->text().trimmed();
    int findIndex = findTabIndexByName(ui->tabWidget, expression);
    if (findIndex!=-1) {
        ui->tabWidget->setCurrentIndex(findIndex);
        return;
    }

    QString error = validateExpression(expression);
    if (!error.isEmpty()){
        QMessageBox::warning(this, "Помилка", error);
        return;
    }

    _tab = new Tab(nullptr, expression, currentCellHoverColor);
    connect(this, &MainWindow::changeCellHoverColorSignal, _tab, &Tab::changeCellHoverColor);
    connect(_tab, &Tab::sendExpressionTypeSignal, this, &MainWindow::setExpressionType);
    _tab->build(expression);

    int index = ui->tabWidget->addTab(_tab, expression);
    ui->tabWidget->setCurrentIndex(index);

    ui->stackedWidget->setCurrentIndex(1);
}



void MainWindow::closeTab(int tabNumber)
{
    QWidget* widget = ui->tabWidget->widget(tabNumber);
    if (!widget)
        return;

    ui->tabWidget->removeTab(tabNumber);

    widget->deleteLater();

    if (ui->tabWidget->count() == 0)
    {
        ui->stackedWidget->setCurrentIndex(0);
        ui->inputLineEdit->clear();
    }
}

void MainWindow::onTabChanged(int index)
{
    QString tabName = ui->tabWidget->tabText(index);
    ui->inputLineEdit->setText(tabName);

    QWidget* widget = ui->tabWidget->widget(index);
    if (!widget)
        return;
    Tab* tab = qobject_cast<Tab*>(widget);
    int type = tab->getExpersionType();

    qDebug()<<"tab changed, type: "<<type;
    switch (type)
    {
    case 0:
        ui->expressionTypeLabel->setText("Тип виразу: тавтологія");
        ui->expressionTypeLabel->setStyleSheet("color: green;");
        return;
    case 1:
        ui->expressionTypeLabel->setText("Тип виразу: протиріччя");
        ui->expressionTypeLabel->setStyleSheet("color: red;");
        return;
    case 2:
        ui->expressionTypeLabel->setText("Тип виразу: нейтральний (виконуваний)");
        ui->expressionTypeLabel->setStyleSheet("color: black;");
        return;
    }
}


void MainWindow::loadSettings() //todo итерироваться по всем виджетам таба, менять там цвет
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

QString MainWindow::validateExpression(const QString &expr) const
{
    if (expr.isEmpty()) {
        return QString("Вираз не може бути порожнім.");
    }

    QString allowedOperators = "!*+>=∨¬∧⇒⇔";
    QSet<QChar> allowedChars;
    for (QChar ch : allowedOperators + "() ") {
        allowedChars.insert(ch);
    }

    for (QChar ch : expr) {
        if (!allowedChars.contains(ch) && !ch.isLetter()) {
            return QString("Вираз містить недопустимі символи: '%1'.").arg(ch);
        }
        if (ch.unicode() >= 0x0400 && ch.unicode() <= 0x04FF) {
            return QString("Вираз містить кириличні символи: '%1'.").arg(ch);
        }
    }

    int balance = 0;
    for (QChar ch : expr) {
        if (ch == '(') balance++;
        else if (ch == ')') balance--;
        if (balance < 0) {
            return QString("Незбалансовані дужки: закриваюча дужка перед відкриваючою.");
        }
    }
    if (balance != 0) {
        return QString("Незбалансовані дужки: не вистачає закриваючих або відкриваючих дужок.");
    }

    for (int i = 0; i < expr.size() - 1; ++i) {
        if (expr[i] == '(' && expr[i + 1] == ')') {
            return QString("Знайдено порожні дужки '()' у виразі.");
        }
    }

    for (int i = 0; i < expr.size(); ++i) {
        QString binaryOperators = "*+>=";
        QChar ch = expr[i];
        if (allowedOperators.contains(ch)) {
            if (i == expr.size() - 1) {
                return QString("Вираз не може закінчуватися оператором '%1'.").arg(ch);
            }
            for (int i = 0; i < expr.size() - 1; ++i) {
                QChar current = expr[i];
                QChar next = expr[i + 1];

                // Якщо поточний символ — бінарний оператор
                if (binaryOperators.contains(current)) {
                    // І наступний символ також бінарний оператор
                    if (binaryOperators.contains(next)) {
                        return QString("Помилка: два бінарних оператори поспіль: '%1' і '%2'.").arg(current).arg(next);
                    }
                }
            }
        }
    }

    QVector<QChar> vars = extractVariables(expr);
    if (vars.isEmpty()) {
        return QString("Вираз не містить змінних.");
    }

    if (vars.size() > 10) {
        return QString("Занадто багато змінних (%1). Максимум дозволено: 10.").arg(vars.size());
    }

    for (int i = 0; i < expr.size() - 1; ++i) {
        if (expr[i] == '!') {
            QChar next = expr[i + 1];
            if (!next.isLetterOrNumber() && next != '(') {
                return QString("Після унарного оператора '!' має бути змінна або відкриваюча дужка.");
            }
        }
    }

    return QString("");
}

QVector<QChar> MainWindow::extractVariables(const QString &expr) const // code duplication, forced measure
{
    QVector<QChar> varList;
    QSet<QChar> varSet;
    for (QChar ch : expr) {
        if (ch.isLetter() && !varSet.contains(ch)) {
            varSet.insert(ch);
            varList.append(ch);
        }
    }
    return varList;
}

int MainWindow::findTabIndexByName(QTabWidget *tabWidget, const QString &tabName)
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == tabName) {
            return i;
        }
    }
    return -1;
}




void MainWindow::closeEvent(QCloseEvent *event)
{
    settings->saveWindowGeometry(saveGeometry());
    QMainWindow::closeEvent(event);
}

void MainWindow::setExpressionType(int type)
{
    qDebug()<<"set type " << type;


    switch (type)
    {
    case 0:
        ui->expressionTypeLabel->setText("Тип виразу: тавтологія");
        ui->expressionTypeLabel->setStyleSheet("color: green;");
    case 1:
        ui->expressionTypeLabel->setText("Тип виразу: протиріччя");
        ui->expressionTypeLabel->setStyleSheet("color: red;");
    case 2:
        ui->expressionTypeLabel->setText("Тип виразу: нейтральний (виконуваний)");
        ui->expressionTypeLabel->setStyleSheet("color: black;");
    }
    //ui->expressionTypeLabel->setText(type);
}


