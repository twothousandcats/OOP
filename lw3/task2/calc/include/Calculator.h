#pragma once
#include <string>
#include "SymbolTable.h"

namespace calc
{

class Calculator
{
public:
	std::string processCommand(const std::string& line);

private:
	SymbolTable m_table;


	std::string handleVar(const std::string& name);

	std::string handleLet(const std::string& line);

	std::string handleFn(const std::string& line);

	std::string handlePrint(const std::string& name) const;

	std::string handlePrintVars();

	std::string handlePrintFns();

	OpType charToOp(char c);
};

} // namespace calc