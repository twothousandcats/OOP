#include "Calculator.h"
#include "utils.h"
#include <sstream>

namespace calc
{

std::string Calculator::ProcessCommand(const std::string& line)
{
	std::string trimmedLine = trim(line);
	if (trimmedLine.empty())
	{
		return "";
	}

	std::istringstream iss(trimmedLine);
	std::string command;
	iss >> command;

	if (command == "var")
	{
		std::string name;
		if (!(iss >> name) || checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}
		return HandleVar(name);
	}

	if (command == "let")
	{
		return HandleLet(trimmedLine);
	}

	if (command == "fn")
	{
		return HandleFn(trimmedLine);
	}

	if (command == "print")
	{
		std::string name;
		if (!(iss >> name) || checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}
		return HandlePrint(name);
	}

	if (command == "printvars")
	{
		if (checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}
		return HandlePrintVars();
	}

	if (command == "printfns")
	{
		if (checkExtraChars(iss))
		{
			return "Invalid usage\n";
		}
		return HandlePrintFns();
	}

	return "Unknown command\n";
}

std::string Calculator::HandleVar(const std::string& name)
{
	if (!isValidIdentifier(name))
	{
		return "Invalid usage\n";
	}
	if (!m_table.DeclareVariable(name))
	{
		return "Name already exists\n";
	}
	return "";
}

std::string Calculator::HandleLet(const std::string& line)
{
	// let <id> = <value|id>
	constexpr std::string_view keyPrefix = "let";
	if (line.substr(0, keyPrefix.size()) != keyPrefix)
	{
		return "Invalid usage\n";
	}

	std::string content = line.substr(keyPrefix.size());
	const auto eqPos = content.find('=');
	if (eqPos == std::string::npos)
	{
		return "Invalid usage\n";
	}

	const std::string target = trim(content.substr(0, eqPos));
	const std::string valuePart = trim(content.substr(eqPos + 1));

	if (!isValidIdentifier(target))
	{
		return "Invalid usage\n";
	}
	if (m_table.IsFunction(target))
	{
		return "Invalid usage\n";
	}

	// try to digit
	char* endPtr = nullptr;
	const double literal = std::strtod(valuePart.c_str(), &endPtr);
	if (*endPtr == '\0' && endPtr != valuePart.c_str() && !valuePart.empty()) // is literal
	{
		if (!m_table.IsVariable(target))
		{
			m_table.DeclareVariable(target);
		}
		m_table.SetVariableValue(target, Value(literal));
		return "";
	}

	// identifier
	if (!isValidIdentifier(valuePart))
	{
		return "Invalid usage\n";
	}
	if (!m_table.HasSymbol(valuePart))
	{
		return "Name does not exist\n";
	}

	if (!m_table.IsVariable(target))
	{
		m_table.DeclareVariable(target);
	}

	const Value val = m_table.IsFunction(valuePart)
		? m_table.EvaluateFunction(valuePart)
		: m_table.GetVariableValue(valuePart);

	m_table.SetVariableValue(target, val);
	return "";
}

std::string Calculator::HandleFn(const std::string& line)
{
	constexpr std::string_view kFnPrefix = "fn";
	if (line.substr(0, kFnPrefix.size()) != kFnPrefix)
	{
		return "Invalid usage\n";
	}

	std::string content = line.substr(kFnPrefix.size());
	const auto eqPos = content.find('=');
	if (eqPos == std::string::npos)
	{
		return "Invalid usage\n";
	}

	std::string target = trim(content.substr(0, eqPos));
	std::string expr = trim(content.substr(eqPos + 1));

	if (!isValidIdentifier(target))
	{
		return "Invalid usage\n";
	}
	if (m_table.HasSymbol(target))
	{
		return "Name already exists\n";
	}

	FunctionDef def;
	def.operation = OperationType::None;
	size_t opPos = std::string::npos;
	char opChar = 0;

	// search operator
	for (size_t i = 0; i < expr.length(); ++i)
	{
		const char ch = expr[i];
		if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
		{
			opPos = i;
			opChar = ch;
			break;
		}
	}

	if (opPos != std::string::npos && opPos > 0 && opPos < expr.length() - 1)
	{
		std::string op1 = trim(expr.substr(0, opPos));
		std::string op2 = trim(expr.substr(opPos + 1));

		if (op1.empty() || op2.empty())
		{
			return "Invalid usage\n";
		}
		if (!isValidIdentifier(op1) || !isValidIdentifier(op2))
		{
			return "Invalid usage\n";
		}
		if (!m_table.HasSymbol(op1) || !m_table.HasSymbol(op2))
		{
			return "Name does not exist\n";
		}

		def.operand1 = std::move(op1);
		def.operand2 = std::move(op2);
		def.operation = CharToOperation(opChar);
	}
	else
	{
		// fn x = y
		if (expr.empty() || !isValidIdentifier(expr))
		{
			return "Invalid usage\n";
		}
		if (!m_table.HasSymbol(expr))
		{
			return "Name does not exist\n";
		}

		def.operand1 = expr;
		def.operation = OperationType::None;
	}

	m_table.DeclareFunction(target, def);
	return "";
}

std::string Calculator::HandlePrint(const std::string& name)
{
	if (!isValidIdentifier(name))
	{
		return "Invalid usage\n";
	}
	if (!m_table.HasSymbol(name))
	{
		return "Name does not exist\n";
	}

	const Value val = m_table.IsFunction(name)
		? m_table.EvaluateFunction(name)
		: m_table.GetVariableValue(name);

	std::ostringstream oss;
	val.Print(oss);
	return oss.str() + "\n";
}

std::string Calculator::HandlePrintVars()
{
	std::ostringstream oss;
	const auto names = m_table.GetSortedVariableNames();
	for (const auto& name : names)
	{
		oss << name << ":";
		m_table.GetVariableValue(name).Print(oss);
		oss << "\n";
	}
	return oss.str();
}

std::string Calculator::HandlePrintFns()
{
	std::ostringstream oss;
	const auto names = m_table.GetSortedFunctionNames();
	for (const auto& name : names)
	{
		oss << name << ":";
		m_table.EvaluateFunction(name).Print(oss);
		oss << "\n";
	}
	return oss.str();
}

OperationType Calculator::CharToOperation(const char ch)
{
	switch (ch)
	{
	case '+':
		return OperationType::Add;
	case '-':
		return OperationType::Sub;
	case '*':
		return OperationType::Mul;
	case '/':
		return OperationType::Div;
	default:
		return OperationType::None;
	}
}

} // namespace calc