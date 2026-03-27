#include "SymbolTable.h"
#include <algorithm>
#include <ranges>

namespace calc
{

void SymbolTable::ResetCache()
{
	m_functionCache.clear();
}

bool SymbolTable::DeclareVariable(const std::string& name)
{
	if (HasSymbol(name))
	{
		return false;
	}
	m_variables[name] = Value::Nan();
	ResetCache();
	return true;
}

bool SymbolTable::HasSymbol(const std::string& name) const
{
	return m_variables.contains(name) || m_functions.contains(name);
}

bool SymbolTable::IsVariable(const std::string& name) const
{
	return m_variables.contains(name);
}

bool SymbolTable::IsFunction(const std::string& name) const
{
	return m_functions.contains(name);
}

void SymbolTable::SetVariableValue(const std::string& name, const Value& val)
{
	m_variables[name] = val;
	ResetCache();
}

Value SymbolTable::GetVariableValue(const std::string& name) const
{
	if (const auto it = m_variables.find(name); it != m_variables.end())
	{
		return it->second;
	}
	return Value::Nan();
}

bool SymbolTable::DeclareFunction(const std::string& name, const FunctionDef& def)
{
	if (HasSymbol(name))
	{
		return false;
	}
	m_functions[name] = def;
	return true;
}

Value SymbolTable::EvaluateFunction(const std::string& name)
{
	if (const auto it = m_functionCache.find(name); it != m_functionCache.end())
	{
		return it->second;
	}

	const auto defIt = m_functions.find(name);
	if (defIt == m_functions.end())
	{
		return Value::Nan();
	}

	const auto& [operand1, operand2, op] = defIt->second;
	if (!HasSymbol(operand1))
	{
		return Value::Nan();
	}

	const Value v1 = IsFunction(operand1)
		? EvaluateFunction(operand1)
		: GetVariableValue(operand1);

	if (op == OperationType::None)
	{
		m_functionCache[name] = v1;
		return v1;
	}

	if (!HasSymbol(operand2))
	{
		return Value::Nan();
	}

	const Value v2 = IsFunction(operand2)
		? EvaluateFunction(operand2)
		: GetVariableValue(operand2);

	Value result = Value::Nan();
	switch (op)
	{
	case OperationType::Add:
		result = Value::Add(v1, v2);
		break;
	case OperationType::Sub:
		result = Value::Sub(v1, v2);
		break;
	case OperationType::Mul:
		result = Value::Mul(v1, v2);
		break;
	case OperationType::Div:
		result = Value::Div(v1, v2);
		break;
	default:
		break;
	}

	m_functionCache[name] = result;
	return result;
}

std::vector<std::string> SymbolTable::GetSortedVariableNames() const
{
	std::vector<std::string> names;
	names.reserve(m_variables.size());
	for (const auto& name : m_variables | std::views::keys)
	{
		names.push_back(name);
	}
	return names;
}

std::vector<std::string> SymbolTable::GetSortedFunctionNames() const
{
	std::vector<std::string> names;
	names.reserve(m_functions.size());
	for (const auto& name : m_functions | std::views::keys)
	{
		names.push_back(name);
	}
	return names;
}

} // namespace calc