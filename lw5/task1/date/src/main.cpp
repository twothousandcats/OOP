#include "CDate.h"
#include <iostream>
#include <sstream>
#include <string>
#include <optional>

void PrintHelp()
{
	std::cout << "Commands:\n"
		<< "  d <DD.MM.YYYY>      - Set date\n"
		<< "  t <timestamp>       - Set date by timestamp (days since 01.01.1970)\n"
		<< "  invalid             - Set invalid date\n"
		<< "  ++ / --             - Increment/Decrement by 1 day\n"
		<< "  + <days>            - Add days\n"
		<< "  - <days>            - Subtract days\n"
		<< "  += <days> / -= <days> - Compound add/subtract\n"
		<< "  diff <DD.MM.YYYY>   - Difference in days\n"
		<< "  ==, !=, <, >...     - Compare with another date\n"
		<< "  weekday             - Print day of week\n"
		<< "  show                - Show current date\n"
		<< "  help                - Show this help\n"
		<< "  quit                - Exit program\n";
}

std::string WeekDayToString(const WeekDay wd)
{
	switch (wd)
	{
	case WeekDay::SUNDAY:
		return "Sunday";
	case WeekDay::MONDAY:
		return "Monday";
	case WeekDay::TUESDAY:
		return "Tuesday";
	case WeekDay::WEDNESDAY:
		return "Wednesday";
	case WeekDay::THURSDAY:
		return "Thursday";
	case WeekDay::FRIDAY:
		return "Friday";
	case WeekDay::SATURDAY:
		return "Saturday";
	default:
		return "Unknown";
	}
}

std::optional<CDate> ParseDateFromStream(std::istringstream& iss)
{
	std::string dateStr;
	if (!(iss >> dateStr) || dateStr.empty())
	{
		return std::nullopt;
	}

	std::istringstream dateIss(dateStr);
	char dot1 = 0, dot2 = 0;
	unsigned day = 0, monthNum = 0, year = 0;
	if (dateIss >> day >> dot1 >> monthNum >> dot2 >> year &&
		dot1 == '.' && dot2 == '.' &&
		monthNum >= 1 && monthNum <= 12)
	{
		try
		{
			if (CDate temp(day, static_cast<Month>(monthNum), year); temp.IsValid())
			{
				return temp;
			}
		}
		catch (...)
		{
		}
	}
	return std::nullopt;
}

void HandleComparison(const CDate& current, const std::string& op, std::istringstream& iss)
{
	const auto otherDateOpt = ParseDateFromStream(iss);
	if (!otherDateOpt.has_value())
	{
		std::cout << "Error: Please specify a valid date in DD.MM.YYYY format\n";
		return;
	}

	const CDate& other = otherDateOpt.value();
	bool result = false;

	if (op == "==")
	{
		result = (current == other);
	}
	else if (op == "!=")
	{
		result = (current != other);
	}
	else if (op == "<")
	{
		result = (current < other);
	}
	else if (op == ">")
	{
		result = (current > other);
	}
	else if (op == "<=")
	{
		result = (current <= other);
	}
	else if (op == ">=")
	{
		result = (current >= other);
	}

	std::cout << "Result: " << std::boolalpha << result << "\n";
}

void HandleArithmetic(CDate& currentDate, const std::string& op, std::istringstream& iss)
{
	int days = 0;
	if (!(iss >> days))
	{
		std::cout << "Error: Please specify number of days\n";
		return;
	}

	if (op == "+" || op == "+=")
	{
		currentDate += days;
	}
	else if (op == "-" || op == "-=")
	{
		currentDate -= days;
	}

	std::cout << "Result: " << currentDate << "\n";
}

void HandleShow(const CDate& currentDate)
{
	std::cout << "Current date: " << currentDate << "\n";
}

void HandleWeekday(const CDate& currentDate)
{
	if (currentDate.IsValid())
	{
		std::cout << "Day of week: " << WeekDayToString(currentDate.GetWeekDay()) << "\n";
	}
	else
	{
		std::cout << "Date is invalid\n";
	}
}

void HandleIncrement(CDate& currentDate)
{
	++currentDate;
	std::cout << "Date after ++: " << currentDate << "\n";
}

void HandleDecrement(CDate& currentDate)
{
	--currentDate;
	std::cout << "Date after --: " << currentDate << "\n";
}

void HandleSetDate(CDate& currentDate, std::istringstream& iss)
{
	if (const auto newDate = ParseDateFromStream(iss); newDate.has_value())
	{
		currentDate = newDate.value();
		std::cout << "Date set to: " << currentDate << "\n";
	}
	else
	{
		std::cout << "Error: Invalid date format or value. Use DD.MM.YYYY\n";
	}
}

void HandleSetTimestamp(CDate& currentDate, std::istringstream& iss)
{
	if (unsigned timestamp = 0; iss >> timestamp)
	{
		if (const CDate temp(timestamp); temp.IsValid())
		{
			currentDate = temp;
			std::cout << "Date set to: " << currentDate << " (timestamp=" << timestamp << ")\n";
		}
		else
		{
			std::cout << "Error: Timestamp out of range\n";
		}
	}
	else
	{
		std::cout << "Error: Please specify timestamp as integer\n";
	}
}

void HandleSetInvalid(CDate& currentDate)
{
	currentDate = CDate(40, Month::JANUARY, 2000);
	std::cout << "Date set to invalid\n";
}

void HandleDiff(const CDate& currentDate, std::istringstream& iss)
{
	const auto otherDateOpt = ParseDateFromStream(iss);
	if (!otherDateOpt.has_value())
	{
		std::cout << "Error: Please specify a valid date in DD.MM.YYYY format\n";
		return;
	}

	int diff = currentDate - otherDateOpt.value();
	std::cout << "Difference (current - other): " << diff << " days\n";
}

bool ProcessCommand(CDate& currentDate, const std::string& line)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	if (command == "quit" || command == "exit" || command == "q")
	{
		std::cout << "Goodbye!\n";
		return false;
	}

	if (command == "help" || command == "h" || command == "?")
	{
		PrintHelp();
	}
	else if (command == "show" || command == "s")
	{
		HandleShow(currentDate);
	}
	else if (command == "weekday" || command == "wd")
	{
		HandleWeekday(currentDate);
	}
	else if (command == "++")
	{
		HandleIncrement(currentDate);
	}
	else if (command == "--")
	{
		HandleDecrement(currentDate);
	}
	else if (command == "+" || command == "-" || command == "+=" || command == "-=")
	{
		HandleArithmetic(currentDate, command, iss);
	}
	else if (command == "d" || command == "date")
	{
		HandleSetDate(currentDate, iss);
	}
	else if (command == "t" || command == "timestamp")
	{
		HandleSetTimestamp(currentDate, iss);
	}
	else if (command == "invalid")
	{
		HandleSetInvalid(currentDate);
	}
	else if (command == "==" || command == "!=" ||
		command == "<" || command == ">" ||
		command == "<=" || command == ">=")
	{
		HandleComparison(currentDate, command, iss);
	}
	else if (command == "diff")
	{
		HandleDiff(currentDate, iss);
	}
	else
	{
		std::cout << "Unknown command: " << command << ". Type 'help' for available commands.\n";
	}

	return true;
}

int main()
{
	CDate currentDate;
	std::cout << "Current date: " << currentDate << "\n"
		<< "Type 'help' for available commands.\n";

	std::string line;
	while (std::cout << "> " && std::getline(std::cin, line))
	{
		if (line.empty())
		{
			continue;
		}

		if (!ProcessCommand(currentDate, line))
		{
			break;
		}
	}

	return 0;
}