#include <QtTest/QtTest>
#include "..//mainwindow.h"

class TestValidateExpression : public QObject
{
    Q_OBJECT

private slots:
    void test_validateExpression_data()
    {
        QTest::addColumn<QString>("expr");
        QTest::addColumn<QString>("expectedMessage");

        QTest::newRow("empty")
            << QString("")
            << QObject::tr("Вираз не може бути порожнім.");

        QTest::newRow("invalid_char")
            << QString("A & B")
            << QObject::tr("Вираз містить недопустимі символи: '%1'.").arg('&');

        QTest::newRow("cyrillic")
            << QString("A∨Б")
            << QObject::tr("Вираз містить кириличні символи: '%1'.").arg(QChar(0x0411));

        QTest::newRow("unbalanced_close_first")
            << QString(")A+B")
            << QObject::tr("Незбалансовані дужки: закриваюча дужка перед відкриваючою.");

        QTest::newRow("missing_close")
            << QString("(A+B")
            << QObject::tr("Незбалансовані дужки: не вистачає закриваючих або відкриваючих дужок.");

        QTest::newRow("empty_parens")
            << QString("A*()")
            << QObject::tr("Знайдено порожні дужки '()' у виразі.");

        QTest::newRow("unexpected_open_after_operand")
            << QString("A(B+C)")
            << QObject::tr("Несподівана відкриваюча дужка.");

        QTest::newRow("two_operands")
            << QString("AB+C")
            << QObject::tr("Два операнди поспіль.");

        QTest::newRow("unary_after_operand")
            << QString("A!")
            << QObject::tr("Несподіваний унарний оператор після операнда.");

        QTest::newRow("binary_at_start")
            << QString("*A+B")
            << QObject::tr("Несподіваний бінарний оператор.");

        QTest::newRow("ends_unexpected")
            << QString("A+")
            << QObject::tr("Вираз закінчується несподівано.");

        QTest::newRow("valid_expr")
            << QString("!(A∧B)⇒C∨D")
            << QString("");
    }

    void testValidateExpression()
    {
        QFETCH(QString, expr);
        QFETCH(QString, expectedMessage);

        MainWindow mw;
        QString result = mw.validateExpression(expr);
        QCOMPARE(result, expectedMessage);
    }
};

QTEST_MAIN(TestValidateExpression)
#include "test_validateexpression.moc"
