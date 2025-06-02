#include "TruthTableBuilder.h"

TruthTableBuilder::TruthTableBuilder() {}

TruthTableBuilder *TruthTableBuilder::getInstance() {
    static TruthTableBuilder _instance;
    qDebug()<<"returning instance " << &_instance;
    return &_instance;
}

void TruthTableBuilder::setExpression(const QString& expr) {
    _infix = normalizeOperators(expr);
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
