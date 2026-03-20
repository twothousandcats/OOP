#include "Calculator.h"
#include "utils.h"
#include <sstream>
#include <vector>

namespace calc
{

std::string Calculator::processCommand(const std::string& line)
{
	std::string trimmedLine = calc::trim(line);
	if (trimmedLine.empty())
	{
		return "";
	}

	std::istringstream iss(trimmedLine);
	std::string command;
	iss >> command;

	if (command == "var")
	{
		// init
		std::string name;
		if (!(iss >> name))
		{
			return "Invalid usage\n";
		}

		if (calc::checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}

		return handleVar(name);
	}
	else if (command == "let")
	{
		// change
		return handleLet(trimmedLine);
	}
	else if (command == "fn")
	{
		return handleFn(trimmedLine);
	}
	else if (command == "print")
	{
		std::string name;
		if (!(iss >> name))
		{
			return "Invalid usage\n";
		}

		if (calc::checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}

		return handlePrint(name);
	}
	else if (command == "printvars")
	{
		if (calc::checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}
		return handlePrintVars();
	}
	else if (command == "printfns")
	{
		if (calc::checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}
		return handlePrintFns();
	}

	return "Unknown command\n";
}

std::string Calculator::handleVar(const std::string& name)
{
	if (!calc::isValidIdentifier(name))
	{
		return "Invalid usage\n";
	}
	if (!m_table.declareVariable(name))
	{
		return "Name already exists\n";
	}
	return ""; // correct
}

std::string Calculator::handleLet(const std::string& line)
{
	// let <identifier> = <value>
	if (line.substr(0, 3) != "let")
	{
		return "Invalid usage\n";
	}

	std::string content = line.substr(3);
	const auto eqPos = content.find('=');
	if (eqPos == std::string::npos)
	{
		return "Invalid usage\n";
	}

	const std::string target = calc::trim(content.substr(0, eqPos)); // identifier
	const std::string valuePart = calc::trim(content.substr(eqPos + 1)); // str after eq

	if (!calc::isValidIdentifier(target))
	{
		return "Invalid usage\n";
	}
	if (m_table.isFunction(target))
	{
		return "Invalid usage\n";
	}

	// parse digit
	char* endPtr;
	const double literal = std::strtod(valuePart.c_str(), &endPtr);
	if (*endPtr == '\0' && endPtr != valuePart.c_str())
	{
		if (!m_table.isVariable(target))
		{
			// if not exist -> create
			m_table.declareVariable(target);
		}
		m_table.setVariableValue(target, Value(literal)); // update v
	}
	else
	{
		if (!calc::isValidIdentifier(valuePart))
		{
			return "Invalid usage\n";
		}
		if (!m_table.hasSymbol(valuePart))
		{
			return "Name does not exist\n";
		}

		if (!m_table.isVariable(target))
		{
			m_table.declareVariable(target);
		}

		const Value val = m_table.isFunction(valuePart)
			? m_table.evaluateFunction(valuePart)
			: m_table.getVariableValue(valuePart);

		m_table.setVariableValue(target, val);
	}
	return "";
}

std::string Calculator::handleFn(const std::string& line)
{
	if (line.substr(0, 2) != "fn")
	{
		return "Invalid usage\n";
	}

	std::string content = line.substr(2);
	auto eqPos = content.find('=');
	if (eqPos == std::string::npos)
	{
		return "Invalid usage\n";
	}

	std::string target = calc::trim(content.substr(0, eqPos));
	std::string expr = calc::trim(content.substr(eqPos + 1));

	if (!calc::isValidIdentifier(target))
	{
		return "Invalid usage\n";
	}
	if (m_table.hasSymbol(target))
	{
		return "Name already exists\n";
	}

	FunctionDef def;
	def.op = OpType::None;
	size_t opPos = std::string::npos;
	char opChar = 0;

	// determine operator
	for (size_t i = 0; i < expr.length(); ++i)
	{
		if (char ch = expr[i]; ch == '+' || ch == '-' || ch == '*' || ch == '/')
		{
			opPos = i;
			opChar = ch;
			break;
		}
	}

	if (opPos != std::string::npos && opPos > 0 && opPos < expr.length() - 1)
	{
		std::string op1 = calc::trim(expr.substr(0, opPos));
		std::string op2 = calc::trim(expr.substr(opPos + 1));

		if (op1.empty() || op2.empty())
		{
			return "Invalid usage\n";
		}
		if (!calc::isValidIdentifier(op1) || !calc::isValidIdentifier(op2))
		{
			return "Invalid usage\n";
		}
		if (!m_table.hasSymbol(op1) || !m_table.hasSymbol(op2))
		{
			return "Name does not exist\n";
		}

		def.operand1 = op1;
		def.operand2 = op2;
		def.op = charToOp(opChar);
	}
	else
	{

		const std::string& op1 = expr;
		if (op1.empty())
		{
			return "Invalid usage\n";
		}
		if (!calc::isValidIdentifier(op1))
		{
			return "Invalid usage\n";
		}
		if (!m_table.hasSymbol(op1))
		{
			return "Name does not exist\n";
		}

		def.operand1 = op1;
		def.op = OpType::None;
	}

	m_table.declareFunction(target, def);
	return "";
}

std::string Calculator::handlePrint(const std::string& name) const
{
	if (!calc::isValidIdentifier(name))
	{
		return "Invalid usage\n";
	}
	if (!m_table.hasSymbol(name))
	{
		return "Name does not exist\n";
	}

	const Value val = m_table.isFunction(name)
		? m_table.evaluateFunction(name)
		: m_table.getVariableValue(name);

	std::ostringstream oss;
	val.print(oss);
	return oss.str() + "\n";
}

std::string Calculator::handlePrintVars()
{
	std::ostringstream oss;
	auto names = m_table.getSortedVariableNames();
	for (const auto& name : names)
	{
		oss << name << ":";
		m_table.getVariableValue(name).print(oss);
		oss << "\n";
	}
	return oss.str();
}

std::string Calculator::handlePrintFns()
{
	std::ostringstream oss;
	auto names = m_table.getSortedFunctionNames();
	for (const auto& name : names)
	{
		oss << name << ":";
		m_table.evaluateFunction(name).print(oss);
		oss << "\n";
	}
	return oss.str();
}

OpType Calculator::charToOp(char c)
{
	switch (c)
	{
	case '+':
		return OpType::Add;
	case '-':
		return OpType::Sub;
	case '*':
		return OpType::Mul;
	case '/':
		return OpType::Div;
	default:
		return OpType::None;
	}
}

} // namespace calc