#ifndef TRUTHTABLEBUILDER_H
#define TRUTHTABLEBUILDER_H

#include <QChar>
#include <QVector>
#include <QStack>
#include <QSet>
#include <QMap>
#include <QDebug>
#include <QRegularExpression>

class TruthTableBuilder {
public:
    static TruthTableBuilder* getInstance();

    void setExpression(const QString& expr);

    bool build();

    int varCount() const;
    int subexprCount() const;
    int rowCount() const;

    const QVector<QChar>& getVarList() const;
    const QVector<QString>& getSubexprList() const;
    const QVector<QVector<int>>& getChildDependencies() const;
    bool value(int row, int col) const;

    ~TruthTableBuilder() = default;


private:
    // singleton
    TruthTableBuilder();
    TruthTableBuilder(const TruthTableBuilder&) = delete;
    TruthTableBuilder& operator=(const TruthTableBuilder&) = delete;

    QString _infix;
    QString _postfix;
    QVector<QChar> _varList;
    QVector<QString> _subexpr;
    QVector<QVector<bool>> _tableData;
    QVector<QVector<int>> _childDeps;

    int precedence(QChar op) const;
    bool applyOp(QChar op, bool a, bool b = false) const;
    QString infixToPostfix(const QString& infix);
    QVector<QChar> extractVariables(const QString& expr) const;
    bool evaluateRow(int row, QVector<bool>& results) const;

    static QString normalizeOperators(const QString& expr);
};

#endif // TRUTHTABLEBUILDER_H
