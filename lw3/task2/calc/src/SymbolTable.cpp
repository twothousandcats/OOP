#include "SymbolTable.h"
#include <algorithm>
#include <ranges>

namespace calc
{

bool SymbolTable::declareVariable(const std::string& name)
{
	if (hasSymbol(name))
	{
		return false;
	}
	m_variables[name] = Value::nan();
	return true;
}

bool SymbolTable::hasSymbol(const std::string& name) const
{
	return m_variables.contains(name) || m_functions.contains(name);
}

bool SymbolTable::isVariable(const std::string& name) const
{
	return m_variables.contains(name);
}

bool SymbolTable::isFunction(const std::string& name) const
{
	return m_functions.contains(name);
}

void SymbolTable::setVariableValue(const std::string& name, const Value& val)
{
	m_variables[name] = val;
}

Value SymbolTable::getVariableValue(const std::string& name) const
{
	if (const auto it = m_variables.find(name); it != m_variables.end())
	{
		return it->second;
	}
	return Value::nan();
}

bool SymbolTable::declareFunction(const std::string& name, const FunctionDef& def)
{
	if (hasSymbol(name))
	{
		return false;
	}
	m_functions[name] = def;
	return true;
}

Value SymbolTable::evaluateFunction(const std::string& name) const
{
	const auto it = m_functions.find(name);
	if (it == m_functions.end())
	{
		return Value::nan();
	}

	const auto& [operand1, operand2, op] = it->second;
	if (!hasSymbol(operand1))
	{
		return Value::nan();
	}

	const Value v1 = isFunction(operand1)
		? evaluateFunction(operand1)
		: getVariableValue(operand1);
	if (op == OpType::None)
	{
		return v1;
	}

	// binary
	if (!hasSymbol(operand2))
	{
		return Value::nan();
	}
	const Value v2 = isFunction(operand2)
		? evaluateFunction(operand2)
		: getVariableValue(operand2);

	switch (op)
	{
	case OpType::Add:
		return Value::add(v1, v2);
	case OpType::Sub:
		return Value::sub(v1, v2);
	case OpType::Mul:
		return Value::mul(v1, v2);
	case OpType::Div:
		return Value::div(v1, v2);
	default:
		return Value::nan();
	}
}

std::vector<std::string> SymbolTable::getSortedVariableNames() const
{
	std::vector<std::string> names;
	for (const auto& name : m_variables | std::views::keys)
	{
		names.push_back(name);
	}
	std::ranges::sort(names);
	return names;
}

std::vector<std::string> SymbolTable::getSortedFunctionNames() const
{
	std::vector<std::string> names;
	for (const auto& name : m_functions | std::views::keys)
	{
		names.push_back(name);
	}
	std::ranges::sort(names);
	return names;
}

} // namespace calc