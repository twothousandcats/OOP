#include "SymbolTable.h"
#include <algorithm>

namespace calc {

bool SymbolTable::declareVariable(const std::string& name) {
    if (hasSymbol(name)) return false;
    m_variables[name] = Value::nan();
    return true;
}

bool SymbolTable::hasSymbol(const std::string& name) const {
    return m_variables.count(name) > 0 || m_functions.count(name) > 0;
}

bool SymbolTable::isVariable(const std::string& name) const {
    return m_variables.count(name) > 0;
}

bool SymbolTable::isFunction(const std::string& name) const {
    return m_functions.count(name) > 0;
}

void SymbolTable::setVariableValue(const std::string& name, const Value& val) {
    m_variables[name] = val;
}

Value SymbolTable::getVariableValue(const std::string& name) const {
    auto it = m_variables.find(name);
    if (it != m_variables.end()) return it->second;
    return Value::nan();
}

bool SymbolTable::declareFunction(const std::string& name, const FunctionDef& def) {
    if (hasSymbol(name)) return false;
    m_functions[name] = def;
    return true;
}

Value SymbolTable::evaluateFunction(const std::string& name) const {
    auto it = m_functions.find(name);
    if (it == m_functions.end()) return Value::nan();

    const auto& def = it->second;
    
    // Получаем значение первого операнда
    if (!hasSymbol(def.operand1)) return Value::nan();
    
    Value v1 = isFunction(def.operand1) ? evaluateFunction(def.operand1) : getVariableValue(def.operand1);

    // Если операция None (простое присваивание fn f = x)
    if (def.op == OpType::None) {
        return v1;
    }

    // Бинарная операция
    if (!hasSymbol(def.operand2)) return Value::nan();
    Value v2 = isFunction(def.operand2) ? evaluateFunction(def.operand2) : getVariableValue(def.operand2);

    switch (def.op) {
        case OpType::Add: return Value::add(v1, v2);
        case OpType::Sub: return Value::sub(v1, v2);
        case OpType::Mul: return Value::mul(v1, v2);
        case OpType::Div: return Value::div(v1, v2);
        default: return Value::nan();
    }
}

std::vector<std::string> SymbolTable::getSortedVariableNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : m_variables) names.push_back(name);
    std::sort(names.begin(), names.end());
    return names;
}

std::vector<std::string> SymbolTable::getSortedFunctionNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : m_functions) names.push_back(name);
    std::sort(names.begin(), names.end());
    return names;
}

} // namespace calc