#include "Calculator.h"
#include "utils.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace calc {

Calculator::Calculator(std::istream& in, std::ostream& out)
    : m_in(in), m_out(out) {}

bool Calculator::isValidIdentifier(const std::string& name) {
    return calc::isValidIdentifier(name);
}

void Calculator::run() {
    std::string line;
    while (std::getline(m_in, line)) {
        if (line.empty()) continue;
        processCommand(line);
    }
}

void Calculator::processCommand(const std::string& line) {
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    if (cmd == "var") {
        std::string name;
        if (!(iss >> name)) {
            m_out << "Invalid usage\n";
            return;
        }
        // Проверка, что больше ничего нет в строке (не обязательно строго, но желательно)
        handleVar(name);
    }
    else if (cmd == "let") {
        handleLet(line);
    }
    else if (cmd == "fn") {
        handleFn(line);
    }
    else if (cmd == "print") {
        std::string name;
        if (!(iss >> name)) {
            m_out << "Invalid usage\n";
            return;
        }
        handlePrint(name);
    }
    else if (cmd == "printvars") {
        handlePrintVars();
    }
    else if (cmd == "printfns") {
        handlePrintFns();
    }
    else {
        m_out << "Unknown command\n";
    }
}

void Calculator::handleVar(const std::string& name) {
    if (!isValidIdentifier(name)) {
        m_out << "Invalid usage\n";
        return;
    }
    if (!m_table.declareVariable(name)) {
        m_out << "Name already exists\n";
    }
}

void Calculator::handleLet(const std::string& line) {
    // Формат: let <target> = <value>
    // Удаляем "let "
    std::string content = line.substr(4);

    // Ищем '='
    auto eqPos = content.find('=');
    if (eqPos == std::string::npos) {
        m_out << "Invalid usage\n";
        return;
    }

    std::string targetStr = content.substr(0, eqPos);
    std::string valueStr = content.substr(eqPos + 1);

    // Trim whitespace
    auto start = targetStr.find_first_not_of(" \t");
    auto end = targetStr.find_last_not_of(" \t");
    if (start == std::string::npos) { m_out << "Invalid usage\n"; return; }
    std::string target = targetStr.substr(start, end - start + 1);

    start = valueStr.find_first_not_of(" \t");
    end = valueStr.find_last_not_of(" \t");
    if (start == std::string::npos) { m_out << "Invalid usage\n"; return; }
    std::string valuePart = valueStr.substr(start, end - start + 1);

    if (!isValidIdentifier(target)) {
        m_out << "Invalid usage\n";
        return;
    }

    // Если target - функция, ошибка
    if (m_table.isFunction(target)) {
        m_out << "Invalid usage\n";
        return;
    }

    // Проверяем, число ли это или идентификатор
    char* endPtr;
    double literal = std::strtod(valuePart.c_str(), &endPtr);

    if (*endPtr == '\0' && endPtr != valuePart.c_str()) {
        // Это число
        // Если переменная не объявлена, объявляем
        if (!m_table.isVariable(target)) {
            m_table.declareVariable(target);
        }
        m_table.setVariableValue(target, Value(literal));
    } else {
        // Это идентификатор
        if (!isValidIdentifier(valuePart)) {
            m_out << "Invalid usage\n";
            return;
        }
        if (!m_table.hasSymbol(valuePart)) {
            m_out << "Name does not exist\n";
            return;
        }
        if (!m_table.isVariable(target)) {
            m_table.declareVariable(target);
        }
        Value val = m_table.isFunction(valuePart) ? m_table.evaluateFunction(valuePart) : m_table.getVariableValue(valuePart);
        m_table.setVariableValue(target, val);
    }
}

void Calculator::handleFn(const std::string& line) {
    // Формат: fn <target> = <expr>
    std::string content = line.substr(3); // remove "fn "

    auto eqPos = content.find('=');
    if (eqPos == std::string::npos) {
        m_out << "Invalid usage\n";
        return;
    }

    std::string targetStr = content.substr(0, eqPos);
    std::string exprStr = content.substr(eqPos + 1);

    // Trim
    auto start = targetStr.find_first_not_of(" \t");
    auto end = targetStr.find_last_not_of(" \t");
    if (start == std::string::npos) { m_out << "Invalid usage\n"; return; }
    std::string target = targetStr.substr(start, end - start + 1);

    start = exprStr.find_first_not_of(" \t");
    end = exprStr.find_last_not_of(" \t");
    if (start == std::string::npos) { m_out << "Invalid usage\n"; return; }
    std::string expr = exprStr.substr(start, end - start + 1);

    if (!isValidIdentifier(target)) {
        m_out << "Invalid usage\n";
        return;
    }

    if (m_table.hasSymbol(target)) {
        m_out << "Name already exists\n";
        return;
    }

    FunctionDef def;
    def.op = OpType::None;

    // Парсинг выражения: ищем оператор +, -, *, /
    // Оператор может быть окружен пробелами или нет.
    // Ищем первый встречающийся оператор, который не является частью идентификатора (но в идентификаторах только _ алфавит цифры)
    // Так что любой + - * / это оператор.

    size_t opPos = std::string::npos;
    char opChar = 0;

    // Ищем операторы в порядке появления, но нужно найти "главный" бинарный оператор.
    // В простом случае a+b или a + b.
    // Пройдемся по строке и найдем первый встреченный оператор, который находится между двумя потенциальными операндами.
    // Для простоты: ищем первый символ из +-*/

    for (size_t i = 0; i < expr.length(); ++i) {
        char c = expr[i];
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            // Убедимся, что это не часть числа (например, отрицательное)?
            // По спецификации операнды - идентификаторы. Числа в fn не поддерживаются напрямую в выражении, только идентификаторы.
            // "fn <id> = <id> <op> <id>"
            opPos = i;
            opChar = c;
            break;
        }
    }

    if (opPos != std::string::npos) {
        // Бинарная операция
        std::string op1 = expr.substr(0, opPos);
        std::string op2 = expr.substr(opPos + 1);

        // Trim op1, op2
        auto s1 = op1.find_first_not_of(" \t");
        auto e1 = op1.find_last_not_of(" \t");
        if (s1 == std::string::npos) { m_out << "Invalid usage\n"; return; }
        op1 = op1.substr(s1, e1 - s1 + 1);

        auto s2 = op2.find_first_not_of(" \t");
        auto e2 = op2.find_last_not_of(" \t");
        if (s2 == std::string::npos) { m_out << "Invalid usage\n"; return; }
        op2 = op2.substr(s2, e2 - s2 + 1);

        if (!isValidIdentifier(op1) || !isValidIdentifier(op2)) {
            m_out << "Invalid usage\n";
            return;
        }

        // Проверка существования операндов
        if (!m_table.hasSymbol(op1) || !m_table.hasSymbol(op2)) {
            m_out << "Name does not exist\n";
            return;
        }

        def.operand1 = op1;
        def.operand2 = op2;
        def.op = charToOp(opChar);
    } else {
        // Унарное присваивание: fn f = x
        std::string op1 = expr;
        auto s1 = op1.find_first_not_of(" \t");
        auto e1 = op1.find_last_not_of(" \t");
        if (s1 == std::string::npos) { m_out << "Invalid usage\n"; return; }
        op1 = op1.substr(s1, e1 - s1 + 1);

        if (!isValidIdentifier(op1)) {
            m_out << "Invalid usage\n";
            return;
        }
        if (!m_table.hasSymbol(op1)) {
            m_out << "Name does not exist\n";
            return;
        }
        def.operand1 = op1;
        def.op = OpType::None;
    }

    m_table.declareFunction(target, def);
}

void Calculator::handlePrint(const std::string& name) {
    if (!isValidIdentifier(name)) {
        m_out << "Invalid usage\n";
        return;
    }
    if (!m_table.hasSymbol(name)) {
        m_out << "Name does not exist\n";
        return;
    }

    Value val;
    if (m_table.isFunction(name)) {
        val = m_table.evaluateFunction(name);
    } else {
        val = m_table.getVariableValue(name);
    }
    val.print(m_out);
    m_out << "\n";
}

void Calculator::handlePrintVars() {
    auto names = m_table.getSortedVariableNames();
    for (const auto& name : names) {
        m_out << name << ":";
        m_table.getVariableValue(name).print(m_out);
        m_out << "\n";
    }
}

void Calculator::handlePrintFns() {
    auto names = m_table.getSortedFunctionNames();
    for (const auto& name : names) {
        m_out << name << ":";
        m_table.evaluateFunction(name).print(m_out);
        m_out << "\n";
    }
}

OpType Calculator::charToOp(char c) {
    switch (c) {
        case '+': return OpType::Add;
        case '-': return OpType::Sub;
        case '*': return OpType::Mul;
        case '/': return OpType::Div;
        default: return OpType::None;
    }
}

} // namespace calc