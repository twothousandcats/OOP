#pragma once
#include <string>
#include <iostream>
#include "Value.h"
#include "SymbolTable.h"

namespace calc {

class Calculator {
public:
	Calculator(std::istream& in, std::ostream& out);
	void run();
	void processCommand(const std::string& line);
	static bool isValidIdentifier(const std::string& name);

private:
	std::istream& m_in;
	std::ostream& m_out;
	SymbolTable m_table;

	void handleVar(const std::string& name);
	void handleLet(const std::string& line);
	void handleFn(const std::string& line);
	void handlePrint(const std::string& name);
	void handlePrintVars();
	void handlePrintFns();

	OpType charToOp(char c);
};

} // namespace calc