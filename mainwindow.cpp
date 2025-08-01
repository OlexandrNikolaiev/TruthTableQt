#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <qactiongroup.h>
#include <QPixmap>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    translator = new QTranslator(this);
    settings = new SettingsManager("settings.ini");

    connect(ui->tabWidget, &QTabWidget::tabBarDoubleClicked, this, &MainWindow::onTabDoubleClicked);

    historyManager = new HistoryManager(ui->menu_history, settings, this);
    connect(historyManager, &HistoryManager::entrySelected, this, &MainWindow::onHistoryEntrySelected);
    historyManager->loadHistory();

    this->setWindowTitle(tr("Конструктор таблиць істинності"));

    ui->inputLineEdit->setPlaceholderText(tr("Введіть логічний вираз..."));
    ui->inputLineEdit->setFocus();

    connect(ui->inputLineEdit,   &QLineEdit::returnPressed,   this, &MainWindow::on_buildButton_clicked);
    connect(ui->calculateButton, &QPushButton::clicked,       this, &MainWindow::on_buildButton_clicked);
    connect(ui->backspaceButton, &QPushButton::clicked, this, [=]() {
        QString text = ui->inputLineEdit->text();
        if (!text.isEmpty()) {
            text.chop(1);
            ui->inputLineEdit->setText(text);
        }
    });

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
        "    padding: 4px 10px;"
        "}"

        "QMenuBar::item:selected {"
        "    background-color: rgb(46, 58, 74);"
        "}"

        "QMenuBar::item:disabled {"
        "    color: rgba(255, 255, 255, 80);"
        "}"

        "QMenu {"
        "    background-color: rgb(40, 50, 64);"
        "    color: white;"
        "    border: 1px solid rgb(60, 70, 85);"
        "}"

        "QMenu::item {"
        "    background-color: transparent;"
        "    padding: 6px 20px;"
        "    margin: 2px 0;"
        "}"

        "QMenu::item:selected {"
        "    background-color: rgb(55, 65, 80);"
        "}"

        "QMenu::item:disabled {"
        "    color: rgba(255, 255, 255, 95);"
        "}"
        );

    QTabBar *tabBar = ui->tabWidget->tabBar();
    QFont font = tabBar->font();
    font.setBold(true);
    font.setPointSize(12);
    tabBar->setFont(font);
    ui->statusbar->setFont(font);

    languageGroup = new QActionGroup(this);
    languageGroup->addAction(ui->actionEnglish);
    languageGroup->addAction(ui->actionUkrainian);
    languageGroup->setExclusive(true);

    connect(ui->actionEnglish, &QAction::triggered, this, &MainWindow::onLanguageActionTriggered);
    connect(ui->actionUkrainian, &QAction::triggered, this, &MainWindow::onLanguageActionTriggered);





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
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    ui->stackedWidget->setCurrentIndex(0);

    ui->welcomeLabel->setText(
        tr("<h1 style='text-align: center;'>Ласкаво просимо до Конструктора таблиць істинності!</h1>"
           "<p style='text-align: center;'>Цей інструмент дозволяє швидко створювати таблиці істинності для логічних виразів. "
           "Вводьте вирази, будуйте таблиці та аналізуйте результати!</p>"
           "<h2 style='text-align: center;'>Як почати?</h2>"
           "<p style='text-align: center;'>"
           "1.  Введіть логічний вираз у поле 'Введіть логічний вираз...' у верхній частині вікна.<br>"
           "2.  Натисніть кнопку 'Побудувати' або клавішу Enter, щоб побудувати таблицю істинності."
           "</p>"
           "<h2 style='text-align: center;'>Бажаєте вдосконалити програму?</h2> "
           "<p style='text-align: center;'>"
           "<a href='https://github.com/OlexandrNikolaiev/TruthTableQt'>Відкритий код проєкту</a>"
           "</p>")
        );


    ui->welcomeLabel_2->setText(
        tr("<h2 style='text-align: left;'>Приклади виразів</h2>"
           "<p style='text-align: left;'>"
           "1.   <span style='font-size: 20px; font-weight: bold;'>A ∧ B</span> — кон'юнкція A та B."
           "</p>"
           "<p style='text-align: left;'>"
           "2.   <span style='font-size: 20px; font-weight: bold;'>¬(A ∨ B)</span> — заперечення диз'юнкції A та B."
           "</p>"
           "<p style='text-align: left;'>"
           "3.   <span style='font-size: 20px; font-weight: bold;'>(A ⇒ B) ⇔ (¬A ∨ B)</span> — еквіваленція імплікації та її розкладення."
           "</p> <br><br><br>")
        );

    ui->welcomeLabel_3->setText(
        tr("<h2 style='text-align: right;'>Гарячі клавіші</h2>"
           "<p style='text-align: right; line-height: 0.7;'>"
           "<span style='font-size: 20px; font-weight: bold;'>'>'</span> — Імплікація.</p>"
           "<p style='text-align: right; line-height: 0.7;'>"
           "<span style='font-size: 20px; font-weight: bold;'>'='</span> — Еквіваленція.</p>"
           "<p style='text-align: right; line-height: 0.7;'>"
           "<span style='font-size: 20px; font-weight: bold;'>Shift + '*'</span> — Кон'юнкція.</p>"
           "<p style='text-align: right; line-height: 0.7;'>"
           "<span style='font-size: 20px; font-weight: bold;'>Shift + '+'</span> — Диз'юнкція.</p>"
           "<p style='text-align: right; line-height: 0.7;'>"
           "<span style='font-size: 20px; font-weight: bold;'>Shift + '!'</span> — Заперечення.</p>"
           "<br><br>")
        );

    QString imagePath = QCoreApplication::applicationDirPath() + "/builder.png";
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        qDebug() << "couldn't load an image";
    } else {
        ui->welcomeImage->setPixmap(pixmap);
    }

    ui->welcomeImage->setPixmap(pixmap.scaled(ui->welcomeImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->welcomeImage->setScaledContents(true);

    ui->welcomeLabel->setWordWrap(true);

    fileManager = new FileManager(ui->tabWidget, this);
    connect(ui->action, &QAction::triggered, fileManager, &FileManager::saveWithDialog);
    connect(ui->action_3, &QAction::triggered, fileManager, &FileManager::loadWithDialog);
    connect(fileManager, &FileManager::sendExpressionFromFile, this, &MainWindow::build);
    connect(fileManager, &FileManager::sendNewTitle, this, &MainWindow::changeWindowTitle);
    connect(fileManager, &FileManager::changeActionStatus, this, &MainWindow::changeSaveAction);
    connect(ui->action_5, &QAction::triggered, fileManager, &FileManager::save);
    connect(this, &MainWindow::changeDataLoaded, fileManager, &FileManager::changeDataLoaded);
    connect(ui->action_8, &QAction::triggered, this, &MainWindow::closeOpenedFile);
    connect(fileManager, &FileManager::clearAllTabs, this, &MainWindow::closeOpenedFile);
    changeSaveAction(false);

    connect(ui->closeProgramAction, &QAction::triggered, this, &MainWindow::close);

    executionTimeLabel = new QLabel(this);
    executionTimeLabel->setStyleSheet("padding-left: 7px; padding-right: 10px;");
    executionTimeLabel->setFont(font);

    ui->statusbar->addWidget(executionTimeLabel);


    connect(ui->action_Excel, &QAction::triggered, this, &MainWindow::exportToExcel);


    int labelWidth = ui->welcomeLabel->width();
    ui->horizontalSpacer_8->changeSize(labelWidth+138, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);


    ui->horizontalLayout->invalidate();
    ui->horizontalLayout->update();

    loadSettings();

    //setWindowState(Qt::WindowMaximized);

    //resources not working
    ui->action_3->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/builder.png"));
    ui->action_Excel->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/excel.png"));

    //workaround, also at onMenuActionTriggered
    QTimer::singleShot(1, this, [this]() {
        this->showMaximized();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTabDoubleClicked(int index)
{
    qDebug() << "Double‑clicked tab index:" << index;
    QWidget *page = ui->tabWidget->widget(index);
    if (!page) return;

    // 1) Приводим к Tab
    Tab *tab = qobject_cast<Tab*>(page);
    if (!tab) {
        qDebug() << "Not a Tab page";
        return;
    }
    qDebug() << "Tab page found";
    tab->restretchTable();

    //table->restr
}

void MainWindow::onHistoryEntrySelected(const QString &expression)
{
    ui->inputLineEdit->setText(expression);
    on_buildButton_clicked();
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
    if (!senderAction) return;

    if (senderAction == ui->action_showAuxButtons) {
        if (checked) {
            qDebug()<<"Aux Checked";
            ui->frame_5->show();
            QTimer::singleShot(1, this, [this]() {
                this->showMaximized();
            });
            //this->showMaximized();
        } else {
            qDebug()<<"Aux not Checked";
            ui->frame_5->hide();
        }
        settings->saveShowAuxButtons(ui->action_showAuxButtons->isChecked());
        return;
    }

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

void MainWindow::onLanguageActionTriggered(bool checked)
{
    QAction *senderAction = qobject_cast<QAction*>(sender());
    if (!senderAction) return;

    if (senderAction == ui->actionEnglish && isUkrainian) {
        if (translator->load("cursova_en_GB.qm")) {
            qApp->installTranslator(translator);
            isUkrainian = false;
            settings->saveLanguage("English");
            qDebug()<<"saving english";

        } else {
            qDebug() << "Failed to load translation file.";
        }
        QEvent languageChangeEvent(QEvent::LanguageChange);
        QApplication::sendEvent(this, &languageChangeEvent);
        QTimer::singleShot(1, this, [this]() {
            this->showMaximized();
        });
    }

    if (senderAction == ui->actionUkrainian && !isUkrainian) {
        qApp->removeTranslator(translator);
        isUkrainian = true;
        settings->saveLanguage("Ukrainian");
        qDebug()<<"saving ukrainian";
        QEvent languageChangeEvent(QEvent::LanguageChange);
        QApplication::sendEvent(this, &languageChangeEvent);
        QTimer::singleShot(1, this, [this]() {
            this->showMaximized();
        });
    }

}

void MainWindow::on_buildButton_clicked() //переробити час виконання коли буде зроблений многопоток
{
    QString expression = ui->inputLineEdit->text().trimmed();
    build(expression, true);
    //emit sendExecutionTime(formattedTime);
}

void MainWindow::closeTab(int tabNumber)
{
    QWidget* widget = ui->tabWidget->widget(tabNumber);
    if (!widget)
        return;

    ui->tabWidget->removeTab(tabNumber);

    widget->deleteLater();

    if (ui->tabWidget->count() == 0) {
        ui->stackedWidget->setCurrentIndex(0);
        ui->inputLineEdit->clear();
        ui->expressionTypeLabel->clear();
        executionTimeLabel->clear();
        //ui->statusbar->clearMessage();
        emit changeDataLoaded(false);
        changeWindowTitle("");
        closeOpenedFile();
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
    int type = tab->getExpressionType();

    //qDebug()<<"tab changed, type: "<<type;

    if (type == 0) {
        ui->expressionTypeLabel->setText(tr("Тип виразу: тавтологія"));
        ui->expressionTypeLabel->setStyleSheet("color: green;");
    } else if(type == 1) {
        ui->expressionTypeLabel->setText(tr("Тип виразу: протиріччя"));
        ui->expressionTypeLabel->setStyleSheet("color: red;");
    } else {
        ui->expressionTypeLabel->setText(tr("Тип виразу: нейтральний (виконуваний)"));
        ui->expressionTypeLabel->setStyleSheet("color: black;");
    }

    executionTimeLabel->setText(tr("Час виконання: ") + tab->getExectuionTime() + tr(" с"));
    //ui->statusbar->showMessage("Час виконання: " + tab->getExectuionTime());
}

void MainWindow::loadSettings()
{
    // QByteArray geometry = settings->loadWindowGeometry(); // make support for different resolutions
    // if (!geometry.isEmpty()) {
    //     qDebug()<<"restoring "<<geometry;
    //     restoreGeometry(geometry);
    // } else {
    //     resize(800, 600);
    //     move(100, 100);
    // }



    bool showAuxButtons = settings->loadShowAuxButtons();
    ui->action_showAuxButtons->setChecked(showAuxButtons);
    if (showAuxButtons) {
        ui->frame_5->show();
    } else {
        ui->frame_5->hide();
    }

    currentCellHoverColor = settings->loadCellHoverColor();
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

    QString currentLanguage = settings->loadLanguage();
    qDebug()<<"loaded currentLanguge: "<<currentLanguage;
    if (currentLanguage=="Ukrainian") {
        ui->actionUkrainian->setChecked(true);
        ui->actionUkrainian->trigger();
    } else {
        ui->actionEnglish->setChecked(true);
        ui->actionEnglish->trigger();
    }
}

QString MainWindow::validateExpression(const QString &expr) const {
    if (expr.isEmpty()) {
        return tr("Вираз не може бути порожнім.");
    }

    QString allowedOperators = "!*+>=∨¬∧⇒⇔";
    QSet<QChar> allowedChars;
    for (QChar ch : allowedOperators + "() ") {
        allowedChars.insert(ch);
    }

    for (QChar ch : expr) {
        if (!allowedChars.contains(ch) && !ch.isLetter()) {
            return tr("Вираз містить недопустимі символи: '%1'.").arg(ch);
        }
        if (ch.unicode() >= 0x0400 && ch.unicode() <= 0x04FF) {
            return tr("Вираз містить кириличні символи: '%1'.").arg(ch);
        }
    }

    int balance = 0;
    for (QChar ch : expr) {
        if (ch == '(') balance++;
        else if (ch == ')') balance--;
        if (balance < 0) {
            return tr("Незбалансовані дужки: закриваюча дужка перед відкриваючою.");
        }
    }
    if (balance != 0) {
        return tr("Незбалансовані дужки: не вистачає закриваючих або відкриваючих дужок.");
    }

    for (int i = 0; i < expr.size() - 1; ++i) {
        if (expr[i] == '(' && expr[i + 1] == ')') {
            return tr("Знайдено порожні дужки '()' у виразі.");
        }
    }

    QString binaryOperators = "*+>=∨∧⇒⇔";
    QString unaryOperators = "!¬";

    QStack<int> parenStack;  // Стек для відстеження відкриваючих дужок
    int state = 0;  // 0: очікуємо операнд або унарний оператор, 1: очікуємо бінарний оператор

    for (int i = 0; i < expr.size(); ++i) {
        QChar ch = expr[i];
        if (ch == '(') {
            parenStack.push(i);
            if (state == 1) {
                return tr("Несподівана відкриваюча дужка.");
            }
            state = 0;  // Всередині дужок очікуємо операнд
        } else if (ch == ')') {
            if (parenStack.isEmpty()) {
                return tr("Незбалансовані дужки.");
            }
            parenStack.pop();
            if (state == 0) {
                return tr("Підвираз закінчується несподівано.");
            }
            state = 1;  // Після дужок очікуємо бінарний оператор або кінець
        } else if (unaryOperators.contains(ch)) {
            if (state == 1) {
                return tr("Несподіваний унарний оператор після операнда.");
            }
            // Унарний оператор не змінює стан
        } else if (ch.isLetter()) {
            if (state == 1) {
                return tr("Два операнди поспіль.");
            }
            state = 1;  // Після операнда очікуємо бінарний оператор
        } else if (binaryOperators.contains(ch)) {
            if (state == 0) {
                return tr("Несподіваний бінарний оператор.");
            }
            state = 0;  // Після бінарного оператора очікуємо операнд
        } else if (!ch.isSpace()) {
            return tr("Недопустимий символ: '%1'.").arg(ch);
        }
    }

    if (state == 0) {
        return tr("Вираз закінчується несподівано.");
    }

    return "";
}

void MainWindow::loadFile(const QString &path)
{
    if (!fileManager->loadFromPath(path)) {
        QMessageBox::warning(this, tr("Помилка"), tr("Не вдалося завантажити файл: %1").arg(path));
    }
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

void MainWindow::build(QString expression, bool addToHistory)
{
    QDateTime startTime = QDateTime::currentDateTime();
    //qDebug()<<startTime;

    int findIndex = findTabIndexByName(ui->tabWidget, expression);

    QString error = validateExpression(expression);
    if (!error.isEmpty()){
        QMessageBox::warning(this, tr("Помилка"), error);
        return;
    }
    if (findIndex!=-1) {
        ui->tabWidget->setCurrentIndex(findIndex);
        return;
    } else {
        if (addToHistory) {
            historyManager->addEntry(expression);
        }
    }

    _tab = new Tab(nullptr, expression, currentCellHoverColor);
    connect(this, &MainWindow::changeCellHoverColorSignal, _tab, &Tab::changeCellHoverColor);
    connect(_tab, &Tab::sendExpressionTypeSignal, this, &MainWindow::setExpressionType);
    connect(_tab, &Tab::statusMessageRequested, this, &MainWindow::changeCurrentOperationText);

    _tab->build(expression);

    int index = ui->tabWidget->addTab(_tab, expression);
    ui->tabWidget->setCurrentIndex(index);

    ui->stackedWidget->setCurrentIndex(1);

    QDateTime endTime = QDateTime::currentDateTime();
    //qDebug()<<endTime;
    qint64 elapsedMs = startTime.msecsTo(endTime);

    int totalSeconds = elapsedMs / 1000;
    //int minutes = elapsedMs / 60000;
    int seconds = (elapsedMs % 60000) / 1000;
    int milliseconds = elapsedMs % 1000;

    QString formattedTime = QString("%1:%2")
                                //.arg(minutes, 2, 10, QChar('0'))
                                .arg(seconds, 2, 10, QChar('0'))
                                .arg(milliseconds, 3, 10, QChar('0'));



    _tab->setExecutionTime(formattedTime);
    executionTimeLabel->setText(tr("Час виконання: ") + formattedTime + tr(" с"));

    ui->action->setEnabled(true);
    ui->action_Excel->setEnabled(true);
    //qDebug()<<formattedTime;
}

// void MainWindow::applyBackgroundColorToMenuBar(const QColor& bgColor) {
//     QString baseRGB = QString("rgb(%1,%2,%3)")
//                           .arg(bgColor.red())
//                           .arg(bgColor.green())
//                           .arg(bgColor.blue());

//     bool isLight = (bgColor.red()*0.299 + bgColor.green()*0.587 + bgColor.blue()*0.114) > 186;

//     QString textColor = isLight ? "black" : "white";

//     QColor selected = isLight ? bgColor.darker(110) : bgColor.lighter(110);
//     QString selectedColor = QString("rgb(%1,%2,%3)")
//                                 .arg(selected.red())
//                                 .arg(selected.green())
//                                 .arg(selected.blue());

//     QColor border = isLight ? QColor(200,200,200) : QColor(60,70,85);
//     QString borderColor = QString("rgb(%1,%2,%3)")
//                               .arg(border.red())
//                               .arg(border.green())
//                               .arg(border.blue());

//     QString style = QString(
//                         "QMenuBar {"
//                         "    background-color: %1;"
//                         "    color: %2;"
//                         "}"
//                         "QMenuBar::item {"
//                         "    background-color: %1;"
//                         "    color: %2;"
//                         "    padding: 4px 10px;"
//                         "}"
//                         "QMenuBar::item:selected {"
//                         "    background-color: %3;"
//                         "}"
//                         "QMenuBar::item:disabled {"
//                         "    color: rgba(0, 0, 0, 80);"
//                         "}"
//                         "QMenu {"
//                         "    background-color: %1;"
//                         "    color: %2;"
//                         "    border: 1px solid %4;"
//                         "}"
//                         "QMenu::item {"
//                         "    background-color: transparent;"
//                         "    padding: 6px 20px;"
//                         "    margin: 2px 0;"
//                         "}"
//                         "QMenu::item:selected {"
//                         "    background-color: %3;"
//                         "}"
//                         "QMenu::item:disabled {"
//                         "    color: rgba(0, 0, 0, 95);"
//                         "}"
//                         ).arg(baseRGB, textColor, selectedColor, borderColor);

//     ui->menubar->setStyleSheet(style);
// }

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<"isFileDataLoaded: " << fileManager->isFileDataLoaded();
    qDebug()<<"table modified: "<<fileManager->isOpenedTableModified();
    settings->saveWindowGeometry(saveGeometry());


    if (fileManager->isFileDataLoaded())
    {

        if (fileManager->isOpenedTableModified())
        {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                tr("Конструктор таблиць істинності"),
                tr("Дані у файлі було змінено. Бажаєте зберігти зміни?"),
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
                );

            if (reply == QMessageBox::Yes) {
                fileManager->save();
                QMainWindow::closeEvent(event);
            } else if (reply == QMessageBox::No) {
                event->accept();
                QMainWindow::closeEvent(event);
            } else if (reply == QMessageBox::Cancel) {
                event->ignore();
                return;
            }
        }
    }
    else {
        QMainWindow::closeEvent(event);
    }
}

void MainWindow::setExpressionType(int type)
{
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
}

void MainWindow::changeCurrentOperationText(QString text)
{
    ui->operationLabel->setText(text);
}

void MainWindow::setStatusBarText(QString text)
{
    ui->statusbar->showMessage(text);
}

void MainWindow::changeWindowTitle(QString newTitle)
{
    this->setWindowTitle(tr("Конструктор таблиць істинності ") + newTitle);
}

void MainWindow::changeSaveAction(bool value)
{
    ui->action->setEnabled(value);
    ui->action_5->setEnabled(value);
    ui->action_8->setEnabled(value);
    ui->action_Excel->setEnabled(value);
}

void MainWindow::closeOpenedFile()
{
    qDebug()<<"closing file";
    int tabCount = ui->tabWidget->count();
    for (int i = tabCount - 1; i >= 0; --i) {
        QWidget* widget = ui->tabWidget->widget(i);
        if (!widget)
            break;
        ui->tabWidget->removeTab(i);
        widget->deleteLater();
    }
    emit changeDataLoaded(false);

    ui->stackedWidget->setCurrentIndex(0);
    executionTimeLabel->clear();
    changeWindowTitle("");
    //changeSaveAction(false);

    changeSaveAction(false);

}

void MainWindow::exportToExcel()
{
    QWidget* currentPage = ui->stackedWidget->currentWidget();
    QString title = ui->inputLineEdit->text().trimmed();

    QTabWidget* tabWidget = currentPage->findChild<QTabWidget*>("tabWidget");
    if (!tabWidget) return;
    QWidget* currentTab = tabWidget->currentWidget();
    QTableWidget* table = currentTab->findChild<QTableWidget*>("truthTable");
    if (!table) return;

    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Експортувати в Excel"),
        QString(),
        tr("Excel файли (*.xlsx *.xls)")
        );
    if (filePath.isEmpty())
        return;

    ExcelExporter exporter;
    QString error;
    if (!exporter.exportToExcel(table, filePath, title, &error)) {
        QMessageBox::warning(this, tr("Помилка"), error);
    } else {
        QMessageBox::information(this, tr("Успіх"), tr("Експорт успішно завершено"));
    }
}

void MainWindow::switchLanguage()
{

}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        ui->inputLineEdit->setPlaceholderText(tr("Введіть логічний вираз..."));
        ui->welcomeLabel->setText(
            tr("<h1 style='text-align: center;'>Ласкаво просимо до Конструктора таблиць істинності!</h1>"
               "<p style='text-align: center;'>Цей інструмент дозволяє швидко створювати таблиці істинності для логічних виразів. "
               "Вводьте вирази, будуйте таблиці та аналізуйте результати!</p>"
               "<h2 style='text-align: center;'>Як почати?</h2>"
               "<p style='text-align: center;'>"
               "1.  Введіть логічний вираз у поле 'Введіть логічний вираз...' у верхній частині вікна.<br>"
               "2.  Натисніть кнопку 'Побудувати' або клавішу Enter, щоб побудувати таблицю істинності."
               "</p>"
               "<h2 style='text-align: center;'>Бажаєте вдосконалити програму?</h2> "
               "<p style='text-align: center;'>"
               "<a href='https://github.com/OlexandrNikolaiev/TruthTableQt'>Відкритий код проєкту</a>"
               "</p>")
            );

        ui->welcomeLabel_2->setText(
            tr("<h2 style='text-align: left;'>Приклади виразів</h2>"
               "<p style='text-align: left;'>"
               "1.   <span style='font-size: 20px; font-weight: bold;'>A ∧ B</span> — кон'юнкція A та B."
               "</p>"
               "<p style='text-align: left;'>"
               "2.   <span style='font-size: 20px; font-weight: bold;'>¬(A ∨ B)</span> — заперечення диз'юнкції A та B."
               "</p>"
               "<p style='text-align: left;'>"
               "3.   <span style='font-size: 20px; font-weight: bold;'>(A ⇒ B) ⇔ (¬A ∨ B)</span> — еквіваленція імплікації та її розкладення."
               "</p> <br><br><br>")
            );

        ui->welcomeLabel_3->setText(
            tr("<h2 style='text-align: right;'>Гарячі клавіші</h2>"
               "<p style='text-align: right; line-height: 0.7;'>"
               "<span style='font-size: 20px; font-weight: bold;'>'>'</span> — Імплікація.</p>"
               "<p style='text-align: right; line-height: 0.7;'>"
               "<span style='font-size: 20px; font-weight: bold;'>'='</span> — Еквіваленція.</p>"
               "<p style='text-align: right; line-height: 0.7;'>"
               "<span style='font-size: 20px; font-weight: bold;'>Shift + '*'</span> — Кон'юнкція.</p>"
               "<p style='text-align: right; line-height: 0.7;'>"
               "<span style='font-size: 20px; font-weight: bold;'>Shift + '+'</span> — Диз'юнкція.</p>"
               "<p style='text-align: right; line-height: 0.7;'>"
               "<span style='font-size: 20px; font-weight: bold;'>Shift + '!'</span> — Заперечення.</p>"
               "<br><br>")
            );

        int labelWidth = ui->welcomeLabel->width();
        ui->horizontalSpacer_8->changeSize(labelWidth-(labelWidth-228), 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        ui->horizontalLayout->invalidate();
        ui->horizontalLayout->update();

        int currentTabIndex = ui->tabWidget->currentIndex();
        if (currentTabIndex != -1) {
            onTabChanged(currentTabIndex);
        } else {
            //qDebug() << "Жодна вкладка не вибрана";
        }

        if (fileManager->isFileDataLoaded()) {
            changeWindowTitle(tr("| Відкрито: ") + fileManager->getFileName());
        }

    }
    QMainWindow::changeEvent(event);
}

