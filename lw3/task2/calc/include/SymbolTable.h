#pragma once

#include <string>
#include <map>
#include <vector>
#include "Value.h"

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
};

class SymbolTable
{
public:
	using VariableMap = std::map<std::string, Value>;
	using FunctionMap = std::map<std::string, FunctionDef>;
	using CacheMap = std::map<std::string, Value>;

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

	void ResetCache();

private:
	VariableMap m_variables;
	FunctionMap m_functions;
	CacheMap m_functionCache;
};

} // namespace calc