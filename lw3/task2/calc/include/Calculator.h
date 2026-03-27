#pragma once

#include <string>
#include "SymbolTable.h"

namespace calc
{

class Calculator
{
public:
	std::string ProcessCommand(const std::string& line);

private:
	std::string HandleVar(const std::string& name);

	std::string HandleLet(const std::string& line);

	std::string HandleFn(const std::string& line);

	std::string HandlePrint(const std::string& name);

	std::string HandlePrintVars();

	std::string HandlePrintFns();

	static OperationType CharToOperation(char ch);

	SymbolTable m_table;
};

} // namespace calc