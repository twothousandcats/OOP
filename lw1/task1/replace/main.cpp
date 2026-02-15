#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <optional>

struct Args
{
	std::string inputPath;
	std::string outputPath;
	std::string searchStr;
	std::string replaceStr;
};

void PrintHelp()
{
	std::cout << "Использование:" << "\n";
	std::cout << "  WIN OS: replace.exe <input file> <output file> <search string> <replace string>" << "\n";
	std::cout << "  MacOS/Linux OS: ./replace <input file> <output file> <search string> <replace string>" << "\n";
	std::cout << "РЕЖИМ \"STDIN\" WIN OS: replace.exe" << "\n";
	std::cout << "РЕЖИМ \"STDIN\" MacOS/Linux OS: ./replace" << "\n";
	std::cout << "  replace.exe -h (показать информацию по использованию)" << "\n";
}

void PrintError()
{
	std::cout << "ERROR" << "\n";
}

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

std::optional<Args> ParseArgs(const int argCount, char* argValues[])
{
	if (argCount != 5)
	{
		return std::nullopt;
	}

	Args args;
	args.inputPath = argValues[1];
	args.outputPath = argValues[2];
	args.searchStr = argValues[3];
	args.replaceStr = argValues[4];

	if (args.inputPath.empty() || args.outputPath.empty())
	{
		return std::nullopt;
	}

	return args;
}

bool OpenFiles(const std::string& inputPath, const std::string& outputPath, std::ifstream& input, std::ofstream& output)
{
	input.open(inputPath);
	if (!input.is_open())
	{
		return false;
	}

	output.open(outputPath);
	if (!output.is_open())
	{
		input.close();
		return false;
	}

	return true;
}

void ProcessStream(std::istream& inpF, std::ostream& outF, const std::string& searchStr, const std::string& replaceStr)
{
	std::string line;
	while (std::getline(inpF, line))
	{
		outF << ReplaceAll(line, searchStr, replaceStr) << "\n";
	}
}

int main(int argCount, char* argValues[])
{
	if (argCount == 2 && std::string(argValues[1]) == "-h")
	{
		PrintHelp();
		return 0;
	}

	if (auto args = ParseArgs(argCount, argValues))
	{
		std::ifstream inputFile;
		std::ofstream outputFile;
		if (!OpenFiles(args->inputPath, args->outputPath, inputFile, outputFile))
		{
			PrintError();
			return 1;
		}

		ProcessStream(inputFile, outputFile, args->searchStr, args->replaceStr);

		inputFile.close();
		outputFile.close();
		return 0;
	}

	if (argCount == 1)
	{
		std::string searchStr;
		if (!std::getline(std::cin, searchStr))
		{
			return 1;
		}
		std::string replaceStr;
		if (!std::getline(std::cin, replaceStr))
		{
			return 1;
		}

		ProcessStream(std::cin, std::cout, searchStr, replaceStr);
		return 0;
	}

	PrintError();
	return 1;
}