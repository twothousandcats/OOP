#pragma once

#include <string>
#include <map>
#include <vector>
#include "Value.h"
#include <unordered_set>
#include <unordered_map>

namespace calc
{

enum class OperationType
{
	None,
	Add,
	Sub,
	Mul,
	Div
};

struct FunctionDef
{
	std::string operand1;
	std::string operand2;
	OperationType operation;
	std::optional<Value> cache;
};

class SymbolTable
{
public:
	// todo: оптимизировать через поиск используемого
	using VariableMap = std::unordered_map<std::string, Value>;
	using FunctionMap = std::unordered_map<std::string, FunctionDef>;
	using DependencyGraph = std::unordered_map<std::string, std::unordered_set<std::string> >;

	bool DeclareVariable(const std::string& name);

	bool HasSymbol(const std::string& name) const;

	bool IsVariable(const std::string& name) const;

	bool IsFunction(const std::string& name) const;

	void SetVariableValue(const std::string& name, const Value& val);

	Value GetVariableValue(const std::string& name) const;

	bool DeclareFunction(const std::string& name, const FunctionDef& def);

	Value EvaluateFunction(const std::string& name);

	std::vector<std::string> GetSortedVariableNames() const;

	std::vector<std::string> GetSortedFunctionNames() const;

private:
	void InvalidateDependentCaches(const std::string& sourceName);

	void RegisterDependencies(const std::string& funcName, const FunctionDef& def);


	VariableMap m_variables;
	FunctionMap m_functions;
	DependencyGraph m_reverseDeps;
};

} // namespace calc