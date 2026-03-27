#pragma once
#include <string>
#include <map>
#include <vector>
#include "Value.h"

namespace calc
{

// operation type
enum class OpType
{
	None,
	Add,
	Sub,
	Mul,
	Div
};

// func view
struct FunctionDef
{
	std::string operand1;
	std::string operand2; // optional cuz (assignment)
	OpType op;
};

class SymbolTable
{
public:
	bool declareVariable(const std::string& name);

	bool hasSymbol(const std::string& name) const;

	bool isVariable(const std::string& name) const;

	bool isFunction(const std::string& name) const;

	void setVariableValue(const std::string& name, const Value& val);

	Value getVariableValue(const std::string& name) const;

	bool declareFunction(const std::string& name, const FunctionDef& def);

	Value evaluateFunction(const std::string& name);

	// list getters
	std::vector<std::string> getSortedVariableNames() const;

	std::vector<std::string> getSortedFunctionNames() const;

	void resetCache();

private:
	// var/fn name as a key
	std::map<std::string, Value> m_variables;
	std::map<std::string, FunctionDef> m_functions;
	std::map<std::string, Value> m_functionCache; // memo
	// mutable std::map<std::string, Value> m_functionCache;
};

} // namespace calc