#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QVector>
#include <QActionGroup>
#include <QScopedPointer>
#include <QGraphicsDropShadowEffect>

#include "TruthTableTab/truthtabletab.h"
#include "IniManager/SettingsManager/settingsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_auxiliaryButton_clicked();
    void onMenuActionTriggered(bool checked);

    void on_buildButton_clicked();

    void closeTab(int tabNumber);
    void onTabChanged(int index);

    void closeEvent(QCloseEvent *event);

    void setExpressionType(int type);


signals:
    void changeCellHoverColorSignal(QColor);

private:
    Ui::MainWindow *ui;

    SettingsManager* settings;

    QColor currentCellHoverColor;
    QActionGroup* colorGroup;
    Tab* _tab;

    void loadSettings();

    QString validateExpression(const QString& expr) const;
    QVector<QChar> extractVariables(const QString& expr) const;
    int findTabIndexByName(QTabWidget *tabWidget, const QString &tabName);


};

#endif // MAINWINDOW_H
