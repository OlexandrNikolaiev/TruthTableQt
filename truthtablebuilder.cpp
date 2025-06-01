// TruthTableBuilder.cpp
#include "truthtablebuilder.h"
#include <stack>
#include <set>
#include <cctype>
#include <unordered_map>
#include <qdebug.h>
#include <algorithm>

TruthTableBuilder::TruthTableBuilder() {}



void TruthTableBuilder::setExpression(const std::string& expr) {

    _infix = normalizeOperators(expr);
}

std::string TruthTableBuilder::validateExpression(const std::string& expr) const {
    if (expr.empty()) {
        return "Вираз не може бути порожнім.";
    }

    // Дозволені символи: латинські літери, оператори, дужки, пробіли
    std::string allowedOperators = "!*+>=";
    std::string allowedChars = allowedOperators + "() ";

    // Перевірка на кириличні символи та некоректні символи
    for (char ch : expr) {
        // if (allowedChars.find(ch) == std::string::npos && !std::isalpha(ch)) {
        //     return "Вираз містить недопустимі символи: '" + std::string(1, ch) + "'.";
        // }
        if (ch >= 0x0400 && ch <= 0x04FF) { // Кириличні символи
            return "Вираз містить кириличні символи: '" + std::string(1, ch) + "'.";
        }
    }

    // Перевірка балансу дужок
    int balance = 0;
    for (char ch : expr) {
        if (ch == '(') balance++;
        else if (ch == ')') balance--;
        if (balance < 0) {
            return "Незбалансовані дужки: закриваюча дужка перед відкриваючою.";
        }
    }
    if (balance != 0) {
        return "Незбалансовані дужки: не вистачає закриваючих або відкриваючих дужок.";
    }

    // Перевірка на порожні дужки
    for (size_t i = 0; i < expr.size() - 1; ++i) {
        if (expr[i] == '(' && expr[i + 1] == ')') {
            return "Знайдено порожні дужки '()' у виразі.";
        }
    }

    // Перевірка на правильність операторів
    for (size_t i = 0; i < expr.size(); ++i) {
        char ch = expr[i];
        if (allowedOperators.find(ch) != std::string::npos) {
            if (i == 0 && ch != '!') {
                return "Вираз не може починатися з бінарного оператора '" + std::string(1, ch) + "'.";
            }
            if (i == expr.size() - 1) {
                return "Вираз не може закінчуватися оператором '" + std::string(1, ch) + "'.";
            }
            if (allowedOperators.find(expr[i + 1]) != std::string::npos) {
                return "Два оператори поспіль: '" + std::string(1, ch) + "' і '" + std::string(1, expr[i + 1]) + "'.";
            }
        }
    }

    // Перевірка на наявність змінних
    std::vector<char> vars = extractVariables(expr);
    if (vars.empty()) {
        return "Вираз не містить змінних.";
    }

    // Обмеження кількості змінних (наприклад, не більше 10)
    if (vars.size() > 10) {
        return "Занадто багато змінних (" + std::to_string(vars.size()) + "). Максимум дозволено: 10.";
    }

    // Перевірка на коректність унарного оператора '!'
    for (size_t i = 0; i < expr.size() - 1; ++i) {
        if (expr[i] == '!') {
            char next = expr[i + 1];
            if (!std::isalnum(next) && next != '(') {
                return "Після унарного оператора '!' має бути змінна або відкриваюча дужка.";
            }
        }
    }

    qDebug()<<"correct";
    return ""; // Вираз коректний
}

int TruthTableBuilder::precedence(char op) const {
    if (op == '!') return 3;
    if (op == '*') return 2;
    if (op == '+') return 1;
    if (op == '>' || op == '=') return 0;
    return -1;
}

bool TruthTableBuilder::applyOp(char op, bool a, bool b) const {
    switch (op) {
    case '!': return !a;
    case '*': return a && b;
    case '+': return a || b;
    case '>': return !a || b;
    case '=': return a == b;
    }
    return false;
}

std::vector<char> TruthTableBuilder::extractVariables(const std::string& expr) const {
    std::set<char> vars;
    for (char ch : expr) {
        if (std::isalnum(ch)) vars.insert(ch);
    }
    return std::vector<char>(vars.begin(), vars.end());
}

std::string TruthTableBuilder::infixToPostfix(const std::string& infix) {
    std::stack<char> ops;
    std::stack<std::string> subStack;
    std::stack<int> idxStack;

    _subexpr.clear();
    _childDeps.clear();

    std::string output;
    int n = (int)_varList.size();

    for (char token : infix) {
        if (token == ' ') continue;

        if (std::isalnum(token)) {
            // Змінна
            output += token;
            subStack.push(std::string(1, token));
            int vidx = int(std::find(_varList.begin(), _varList.end(), token) - _varList.begin());
            idxStack.push(vidx);
        }
        else if (token == '(') {
            ops.push(token);
        }
        else if (token == ')') {
            while (!ops.empty() && ops.top() != '(') {
                char op = ops.top(); ops.pop();
                output += op;

                if (op == '!') {
                    // унарний
                    std::string operand = subStack.top(); subStack.pop();
                    int cidx = idxStack.top(); idxStack.pop();
                    std::string se = "!" + operand;
                    _subexpr.push_back(se);
                    _childDeps.push_back({ cidx });
                    int myIdx = n + (int)_childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                } else {
                    // бінарний
                    std::string right = subStack.top(); subStack.pop();
                    std::string left  = subStack.top(); subStack.pop();
                    int ridx = idxStack.top(); idxStack.pop();
                    int lidx = idxStack.top(); idxStack.pop();
                    std::string se = "(" + left + op + right + ")";
                    _subexpr.push_back(se);
                    _childDeps.push_back({ lidx, ridx });
                    int myIdx = n + (int)_childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                }
            }
            if (!ops.empty()) ops.pop();  // зняли '('
        }
        else {
            // оператор +,*,>,=
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                char op = ops.top(); ops.pop();
                output += op;

                if (op == '!') {
                    std::string operand = subStack.top(); subStack.pop();
                    int cidx = idxStack.top(); idxStack.pop();
                    std::string se = "!" + operand;
                    _subexpr.push_back(se);
                    _childDeps.push_back({ cidx });
                    int myIdx = n + (int)_childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                } else {
                    std::string right = subStack.top(); subStack.pop();
                    std::string left  = subStack.top(); subStack.pop();
                    int ridx = idxStack.top(); idxStack.pop();
                    int lidx = idxStack.top(); idxStack.pop();
                    std::string se = "(" + left + op + right + ")";
                    _subexpr.push_back(se);
                    _childDeps.push_back({ lidx, ridx });
                    int myIdx = n + (int)_childDeps.size() - 1;
                    subStack.push(se);
                    idxStack.push(myIdx);
                }
            }
            ops.push(token);
            // **УВАГА** більше не робимо output += token тут!
        }
    }

    // Очищаємо стек операторів
    while (!ops.empty()) {
        char op = ops.top(); ops.pop();
        output += op;

        if (op == '!') {
            std::string operand = subStack.top(); subStack.pop();
            int cidx = idxStack.top(); idxStack.pop();
            std::string se = "!" + operand;
            _subexpr.push_back(se);
            _childDeps.push_back({ cidx });
            int myIdx = n + (int)_childDeps.size() - 1;
            subStack.push(se);
            idxStack.push(myIdx);
        } else {
            std::string right = subStack.top(); subStack.pop();
            std::string left  = subStack.top(); subStack.pop();
            int ridx = idxStack.top(); idxStack.pop();
            int lidx = idxStack.top(); idxStack.pop();
            std::string se = "(" + left + op + right + ")";
            _subexpr.push_back(se);
            _childDeps.push_back({ lidx, ridx });
            int myIdx = n + (int)_childDeps.size() - 1;
            subStack.push(se);
            idxStack.push(myIdx);
        }
    }

    return output;
}

bool TruthTableBuilder::evaluateRow(int row, std::vector<bool>& results) const {
    int n = _varList.size();
    for (int j = 0; j < n; ++j)
        results[j] = (row & (1 << (n - j - 1))) != 0;

    std::unordered_map<char,bool> vals;
    for (int j = 0; j < n; ++j)
        vals[_varList[j]] = results[j];

    std::stack<bool> st;
    int idx = 0;
    for (char token : _postfix) {
        if (std::isalnum(token)) {
            st.push(vals.at(token));
        } else {
            if (token == '!') {
                bool a = st.top(); st.pop();
                bool r = applyOp(token, a);
                st.push(r);
                results[n + idx++] = r;
            } else {
                bool b = st.top(); st.pop();
                bool a = st.top(); st.pop();
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

    _tableData.assign(rows, std::vector<bool>(n + m));
    for (int i = 0; i < rows; ++i) {
        std::vector<bool> row(n + m);
        evaluateRow(i, row);
        _tableData[i] = std::move(row);
    }
    return true;
}

int TruthTableBuilder::varCount()    const { return (int)_varList.size(); }
int TruthTableBuilder::subexprCount()const { return (int)_childDeps.size(); }
int TruthTableBuilder::rowCount()    const { return (int)_tableData.size(); }
const std::vector<char>& TruthTableBuilder::getVarList() const { return _varList; }
const std::vector<std::string>& TruthTableBuilder::getSubexprList() const { return _subexpr; }
const std::vector<std::vector<int>>& TruthTableBuilder::getChildDependencies() const { return _childDeps; }
bool TruthTableBuilder::value(int row, int col)   const { return _tableData[row][col]; }
