#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <optional>

enum class Operation
{
	Pack,
	Unpack,
};

struct Args
{
	Operation operation;
	std::string inputPath;
	std::string outputPath;
};

void PrintHelp()
{
	std::cout << "RLE compression utility\n";
	std::cout << "Usage:\n";
	std::cout << "  Pack: rle.exe pack <input file> <output file>\n";
	std::cout << "  Unpack: rle.exe unpack <input file> <output file>\n";
	std::cout << "  Help: rle.exe -h\n";
}

void PrintError()
{
	std::cout << "ERROR\n";
}

std::optional<Args> ParseArgs(const int argCount, char* argValues[])
{
	if (argCount != 4)
	{
		return std::nullopt;
	}

	Args args;
	if (const std::string operationStr = argValues[1]; operationStr == "pack")
	{
		args.operation = Operation::Pack;
	}
	else if (operationStr == "unpack")
	{
		args.operation = Operation::Unpack;
	}
	else
	{
		return std::nullopt;
	}

	args.inputPath = argValues[2];
	args.outputPath = argValues[3];

	if (args.inputPath.empty() || args.outputPath.empty())
	{
		return std::nullopt;
	}

	return args;
}

bool OpenFiles(
	const std::string& inputPath,
	const std::string& outputPath,
	std::ifstream& input,
	std::ofstream& output
	)
{
	input.open(inputPath, std::ios::binary);
	if (!input.is_open())
	{
		return false;
	}

	output.open(outputPath, std::ios::binary);
	if (!output.is_open())
	{
		input.close();
		return false;
	}

	return true;
}

void Pack(std::istream& input, std::ostream& output)
{
	char currentByte;
	char lastByte = 0;
	int count = 0;

	bool first = true;
	while (input.get(currentByte))
	{
		if (first)
		{
			lastByte = currentByte;
			count = 1;
			first = false;
			continue;
		}

		if (currentByte == lastByte && count < 255)
		{
			count++;
		}
		else
		{
			output.put(static_cast<char>(count));
			output.put(lastByte);

			lastByte = currentByte;
			count = 1;
		}
	}

	if (!first)
	{
		// There was at least one byte
		output.put(static_cast<char>(count));
		output.put(lastByte);
	}
}

bool Unpack(std::istream& input, std::ostream& output)
{
	char countByte, valueByte;
	while (input.get(countByte))
	{
		const unsigned char count = static_cast<unsigned char>(countByte);
		if (count == 0)
		{
			return false; // Invalid count
		}

		if (!input.get(valueByte))
		{
			return false; // Unexpected end of file
		}

		for (int i = 0; i < count; ++i)
		{
			output.put(valueByte);
		}
	}

	return true;
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

		if (args->operation == Operation::Pack)
		{
			Pack(inputFile, outputFile);
		}
		else
		{
			if (!Unpack(inputFile, outputFile))
			{
				inputFile.close();
				outputFile.close();
				PrintError();
				return 1;
			}
		}

		inputFile.close();
		outputFile.close();
		return 0;
	}

	PrintError();
	return 1;
}