#include "TruthTableBuilder.h"

TruthTableBuilder::TruthTableBuilder() {}

void TruthTableBuilder::setExpression(const QString& expr) {
    _infix = normalizeOperators(expr);
}

QString TruthTableBuilder::validateExpression(const QString& expr) const {
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

int TruthTableBuilder::precedence(QChar op) const {
    if (op == '!') return 3;
    if (op == '*') return 2;
    if (op == '+') return 1;
    if (op == '>' || op == '=') return 0;
    return -1;
}

bool TruthTableBuilder::applyOp(QChar op, bool a, bool b) const {
    switch (op.toLatin1()) {
    case '!': return !a;
    case '*': return a && b;
    case '+': return a || b;
    case '>': return !a || b;
    case '=': return a == b;
    }
    return false;
}

QVector<QChar> TruthTableBuilder::extractVariables(const QString& expr) const { // todo remake
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

QString TruthTableBuilder::infixToPostfix(const QString& infix) {
    QStack<QChar> ops;
    QStack<QString> subStack;
    QStack<int> idxStack;

    _subexpr.clear();
    _childDeps.clear();

    QString output;
    int n = _varList.size();

    for (QChar token : infix) {
        if (token.isSpace()) continue;

        if (token.isLetterOrNumber()) {
            output += token;
            subStack.push(QString(token));
            int vidx = _varList.indexOf(token);
            idxStack.push(vidx);
        }
        else if (token == '(') {
            ops.push(token);
        }
        else if (token == ')') {
            while (!ops.isEmpty() && ops.top() != '(') {
                QChar op = ops.pop();
                output += op;

                if (op == '!') {
                    QString operand = subStack.pop();
                    int cidx = idxStack.pop();
                    QString se = "!" + operand;
                    _subexpr.push_back(se);
                    _childDeps.push_back({cidx});
                    int myIdx = n + _childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                } else {
                    QString right = subStack.pop();
                    QString left = subStack.pop();
                    int ridx = idxStack.pop();
                    int lidx = idxStack.pop();
                    QString se = "(" + left + op + right + ")";
                    _subexpr.push_back(se);
                    _childDeps.push_back({lidx, ridx});
                    int myIdx = n + _childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                }
            }
            if (!ops.isEmpty()) ops.pop();
        }
        else {
            while (!ops.isEmpty() && precedence(ops.top()) >= precedence(token)) {
                QChar op = ops.pop();
                output += op;

                if (op == '!') {
                    QString operand = subStack.pop();
                    int cidx = idxStack.pop();
                    QString se = "!" + operand;
                    _subexpr.push_back(se);
                    _childDeps.push_back({cidx});
                    int myIdx = n + _childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                } else {
                    QString right = subStack.pop();
                    QString left = subStack.pop();
                    int ridx = idxStack.pop();
                    int lidx = idxStack.pop();
                    QString se = "(" + left + op + right + ")";
                    _subexpr.push_back(se);
                    _childDeps.push_back({lidx, ridx});
                    int myIdx = n + _childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                }
            }
            ops.push(token);
        }
    }

    while (!ops.isEmpty()) {
        QChar op = ops.pop();
        output += op;

        if (op == '!') {
            QString operand = subStack.pop();
            int cidx = idxStack.pop();
            QString se = "!" + operand;
            _subexpr.push_back(se);
            _childDeps.push_back({cidx});
            int myIdx = n + _childDeps.size() - 1;
            subStack.push(se);
            idxStack.push(myIdx);
        } else {
            QString right = subStack.pop();
            QString left = subStack.pop();
            int ridx = idxStack.pop();
            int lidx = idxStack.pop();
            QString se = "(" + left + op + right + ")";
            _subexpr.push_back(se);
            _childDeps.push_back({lidx, ridx});
            int myIdx = n + _childDeps.size() - 1;
            subStack.push(se);
            idxStack.push(myIdx);
        }
    }

    return output;
}

bool TruthTableBuilder::evaluateRow(int row, QVector<bool>& results) const {
    int n = _varList.size();
    for (int j = 0; j < n; ++j)
        results[j] = (row & (1 << (n - j - 1))) != 0;

    QMap<QChar, bool> vals;
    for (int j = 0; j < n; ++j)
        vals[_varList[j]] = results[j];

    QStack<bool> st;
    int idx = 0;
    for (QChar token : _postfix) {
        if (token.isLetterOrNumber()) {
            st.push(vals.value(token));
        } else {
            if (token == '!') {
                bool a = st.pop();
                bool r = applyOp(token, a);
                st.push(r);
                results[n + idx++] = r;
            } else {
                bool b = st.pop();
                bool a = st.pop();
                bool r = applyOp(token, a, b);
                st.push(r);
                results[n + idx++] = r;
            }
        }
    }
    return st.top();
}

bool TruthTableBuilder::build() {
    _varList = extractVariables(_infix);
    _postfix = infixToPostfix(_infix);
    int n = _varList.size();
    int m = _childDeps.size();
    int rows = 1 << n;

    _tableData.clear();

    for (int i = 0; i < rows; ++i) {
        _tableData.append(QList<bool>(n + m));
    }

    for (int i = 0; i < rows; ++i) {
        evaluateRow(i, _tableData[i]);
    }

    return true;
}

int TruthTableBuilder::varCount() const { return _varList.size(); }
int TruthTableBuilder::subexprCount() const { return _childDeps.size(); }
int TruthTableBuilder::rowCount() const { return _tableData.size(); }
const QVector<QChar>& TruthTableBuilder::getVarList() const { return _varList; }
const QVector<QString>& TruthTableBuilder::getSubexprList() const { return _subexpr; }
const QVector<QVector<int>>& TruthTableBuilder::getChildDependencies() const { return _childDeps; }
bool TruthTableBuilder::value(int row, int col) const { return _tableData[row][col]; }

QString TruthTableBuilder::normalizeOperators(const QString& expr) {
    QString s = expr;
    s.replace("⇔", "=");
    s.replace("↔", "=");
    s.replace("⇒", ">");
    s.replace("→", ">");
    s.replace("∨", "+");
    s.replace("∧", "*");
    s.replace("¬", "!");
    return s;
}
