#ifndef TRUTHTABLEBUILDER_H
#define TRUTHTABLEBUILDER_H

#include <string>
#include <vector>

class TruthTableBuilder {
public:
    TruthTableBuilder();
    void setExpression(const std::string& expr);
    bool build();

    int varCount() const;
    int subexprCount() const;
    int rowCount() const;

    const std::vector<char>& getVarList() const;
    const std::vector<std::string>& getSubexprList() const;
    const std::vector<std::vector<int>>& getChildDependencies() const;
    bool value(int row, int col) const;

private:
    std::string _infix;
    std::string _postfix;
    std::vector<char> _varList;
    std::vector<std::string> _subexpr;
    std::vector<std::vector<bool>> _tableData;
    std::vector<std::vector<int>> _childDeps;

    int precedence(char op) const;
    bool applyOp(char op, bool a, bool b = false) const;
    std::string infixToPostfix(const std::string& infix);
    std::vector<char> extractVariables(const std::string& expr) const;
    bool evaluateRow(int row, std::vector<bool>& results) const;

    static std::string replaceAll(std::string s, const std::string& from, const std::string& to)
    {
        size_t pos;
        while ((pos = s.find(from)) != std::string::npos) {
            s.replace(pos, from.size(), to);
        }
        return s;
    }

    static std::string normalizeOperators(const std::string& expr) {
        std::string s = expr;
        s = replaceAll(s, "⇔", "=");
        s = replaceAll(s, "↔", "=");
        s = replaceAll(s, "⇒", ">");
        s = replaceAll(s, "→", ">");
        s = replaceAll(s, "∨", "+");
        s = replaceAll(s, "∧", "*");
        s = replaceAll(s, "¬", "!");
        return s;
    }
};

#endif // TRUTHTABLEBUILDER_H
