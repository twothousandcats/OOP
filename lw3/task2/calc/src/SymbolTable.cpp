#include "SymbolTable.h"
#include <ranges>

namespace calc
{

void SymbolTable::RegisterDependencies(const std::string& funcName, const FunctionDef& def)
{
	m_reverseDeps[def.operand1].insert(funcName);
	if (def.operation != OperationType::None)
	{
		m_reverseDeps[def.operand2].insert(funcName);
	}
}

// iterative via queue for dependent fns
void SymbolTable::InvalidateDependentCaches(const std::string& sourceName)
{
	std::queue<std::string> queue;
	queue.push(sourceName);
	std::unordered_set<std::string> visited;
	visited.insert(sourceName);
	while (!queue.empty())
	{
		const std::string current = queue.front();
		queue.pop();
		if (auto it = m_reverseDeps.find(current); it != m_reverseDeps.end())
		{
			for (const auto& dependentFuncName : it->second)
			{
				if (auto funcIt = m_functions.find(dependentFuncName); funcIt != m_functions.end())
				{
					funcIt->second.cache.reset();
					if (visited.insert(dependentFuncName).second)
					{
						queue.push(dependentFuncName);
					}
				}
			}
		}
	}
}

bool SymbolTable::DeclareVariable(const std::string& name)
{
	if (HasSymbol(name))
	{
		return false;
	}
	m_variables[name] = Value::Nan();
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
	if (!m_variables.contains(name))
	{
		return;
	}

	m_variables[name] = val;
	InvalidateDependentCaches(name);
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
	RegisterDependencies(name, def);
	return true;
}

Value SymbolTable::EvaluateFunction(const std::string& name)
{
	const auto it = m_functions.find(name);
	if (it == m_functions.end())
	{
		return Value::Nan();
	}

	FunctionDef& def = it->second;

	auto& [operand1, operand2, op, cache] = def;

	// checking cache
	if (cache.has_value())
	{
		return cache.value();
	}

	if (!HasSymbol(operand1))
	{
		return Value::Nan();
	}

	const Value v1 = IsFunction(operand1)
		? EvaluateFunction(operand1)
		: GetVariableValue(operand1);

	if (op == OperationType::None)
	{
		cache = v1;
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

	cache = result;
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