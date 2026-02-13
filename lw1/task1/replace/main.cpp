#include <iostream>
#include <fstream>
#include <string>
#include <vector>

constexpr int PROGRAM_POS = 0;
constexpr int INPUT_FILE_POS = 1;
constexpr int OUTPUT_FILE_POS = 2;
constexpr int SEARCH_STR_POS = 3;
constexpr int REPLACE_STR_POS = 4;
constexpr int INFO_STR_POS = INPUT_FILE_POS;

constexpr int STDIN_MODE_COUNT = 1;
constexpr int INFO_MODE_COUNT = 2;
constexpr int COMMAND_LINE_MODE_COUNT = 5;

constexpr auto* HELP_FLAG = "-h";
constexpr auto* ERROR_MESSAGE = "ERROR";
constexpr auto* USAGE_HEADING_STR = "Использование:";
constexpr auto* USAGE_COMMAND_LINE_MODE_MSG_WIN = "  WIN OS: replace.exe <input file> <output file> <search string> <replace string>";
constexpr auto* USAGE_COMMAND_LINE_MODE_MSG_UNIX = "  MacOS/Linux OS: ./replace <input file> <output file> <search string> <replace string>";
constexpr auto* USAGE_STDIN_MODE_MSG_WIN = "РЕЖИМ \"STDIN\" WIN OS: replace.exe";
constexpr auto* USAGE_STDIN_MODE_MSG_UNIX = "РЕЖИМ \"STDIN\" MacOS/Linux OS: ./replace";
constexpr auto* USAGE_INFO_MODE_STR = "  replace.exe -h (показать информацию по использованию)";

std::string ReplaceAll(const std::string& str, const std::string& strFrom, const std::string& strTo)
{
	if (strFrom.empty())
	{
		return str;
	}

	size_t pos;
	size_t startPos = 0;
	std::string resultStr;
	while ((pos = str.find(strFrom, startPos)) != std::string::npos)
	{
		resultStr.append(str, startPos, pos - startPos); // Copy the part of the line until found
		resultStr += strTo; // Add a replacement string
		startPos = pos + strFrom.length(); // Update the position
	}
	resultStr.append(str, startPos, str.length() - startPos); // Add a tail

	return resultStr;
}

void PrintHelp()
{
	std::cout << USAGE_HEADING_STR << "\n";
	std::cout << USAGE_COMMAND_LINE_MODE_MSG_WIN << "\n";
	std::cout << USAGE_COMMAND_LINE_MODE_MSG_UNIX << "\n";
	std::cout << USAGE_STDIN_MODE_MSG_WIN << "\n";
	std::cout << USAGE_STDIN_MODE_MSG_UNIX << "\n";
	std::cout << USAGE_INFO_MODE_STR << "\n";
}

int main(int argCount, char* argValues[])
{
	if (argCount == INFO_MODE_COUNT && std::string(argValues[INFO_STR_POS]) == HELP_FLAG)
	{
		PrintHelp();
		return 0;
	}

	if (argCount == COMMAND_LINE_MODE_COUNT)
	{
		// Command line mode
		std::ifstream inputFile(argValues[INPUT_FILE_POS]);
		if (!inputFile.is_open())
		{
			std::cout << ERROR_MESSAGE << "\n";
			return 1;
		}

		std::ofstream outputFile(argValues[OUTPUT_FILE_POS]);
		if (!outputFile.is_open())
		{
			std::cout << ERROR_MESSAGE << "\n";
			return 1;
		}

		std::string searchStr = argValues[SEARCH_STR_POS];
		std::string replaceStr = argValues[REPLACE_STR_POS];

		std::string line;
		while (std::getline(inputFile, line))
		{
			outputFile << ReplaceAll(line, searchStr, replaceStr) << "\n";
		}

		inputFile.close();
		outputFile.close();
		return 0;
	}

	if (argCount == STDIN_MODE_COUNT)
	{
		// Stdin mode
		std::string searchStr;
		if (!std::getline(std::cin, searchStr))
		{
			std::cout << ERROR_MESSAGE << "\n";
			return 0;
		}

		std::string replaceStr;
		if (!std::getline(std::cin, replaceStr))
		{
			std::cout << ERROR_MESSAGE << "\n";
			return 0;
		}

		std::string line;
		while (std::getline(std::cin, line))
		{
			std::cout << ReplaceAll(line, searchStr, replaceStr) << "\n";
		}
		return 0;
	}

	std::cout << ERROR_MESSAGE << "\n";
	return 1;
}