#include "SymbolTable.h"
#include <algorithm>
#include <ranges>

namespace calc
{

void SymbolTable::resetCache()
{
	m_functionCache.clear();
}

bool SymbolTable::declareVariable(const std::string& name)
{
	if (hasSymbol(name))
	{
		return false;
	}
	m_variables[name] = Value::nan();
	resetCache();
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
	resetCache();
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

Value SymbolTable::evaluateFunction(const std::string& name)
{
	// checking cache
	if (const auto it = m_functionCache.find(name); it != m_functionCache.end())
	{
		return it->second;
	}

	// searching defined
	const auto defIt = m_functions.find(name);
	if (defIt == m_functions.end())
	{
		return Value::nan();
	}

	const auto& [operand1, operand2, op] = defIt->second;
	if (!hasSymbol(operand1))
	{
		return Value::nan();
	}

	const Value v1 = isFunction(operand1)
		? evaluateFunction(operand1)
		: getVariableValue(operand1);

	if (op == OpType::None)
	{
		m_functionCache[name] = v1;
		return v1;
	}

	if (!hasSymbol(operand2))
	{
		return Value::nan();
	}

	const Value v2 = isFunction(operand2)
		? evaluateFunction(operand2)
		: getVariableValue(operand2);

	Value result = Value::nan();
	switch (op)
	{
	case OpType::Add:
		result = Value::add(v1, v2);
		break;
	case OpType::Sub:
		result = Value::sub(v1, v2);
		break;
	case OpType::Mul:
		result = Value::mul(v1, v2);
		break;
	case OpType::Div:
		result = Value::div(v1, v2);
		break;
	default:
		break;
	}

	m_functionCache[name] = result;

	return result;
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