#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <climits>
#include <cctype>

constexpr int MIN_RADIX = 2;
constexpr int MAX_RADIX = 36;

struct Args
{
	int sourceRadix{};
	int destRadix{};
	std::string value;
};

void PrintHelp()
{
	std::cout << "Usage:\n";
	std::cout << "  WinOS: radix.exe <source notation> <destination notation> <value>\n";
	std::cout << "  MacOS/Linux: ./radix <source notation> <destination notation> <value>\n";
	std::cout << "Example: radix.exe 16 10 1F\n";
	std::cout << "  -h (show usage)\n";
}

void PrintError(const std::string& msg)
{
	std::cout << "ERROR: " << msg << "\n";
}

// ascii code
int CharToInt(const char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		return ch - '0';
	}
	if (ch >= 'A' && ch <= 'Z')
	{
		return ch - 'A' + 10;
	}
	return -1;
}

// symbol by ascii code
char IntToChar(const int n)
{
	if (n >= 0 && n <= 9)
	{
		return static_cast<char>('0' + n);
	}
	if (n >= 10 && n <= 35)
	{
		return static_cast<char>('A' + n - 10);
	}
	return '\0';
}

int StringToInt(const std::string& str, const int radix, bool& wasError)
{
	wasError = false;
	if (str.empty())
	{
		wasError = true;
		return 0;
	}

	size_t start = 0;
	bool negative = false;
	if (str[0] == '-' || str[0] == '+')
	{
		if (str.length() == 1)
		{
			wasError = true;
			return 0;
		}
		start = 1;
		if (str[0] == '-')
		{
			negative = true;
		}
	}

	for (size_t i = start; i < str.length(); ++i)
	{
		if (const char ch = str[i]; !((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z')))
		{
			wasError = true;
			return 0;
		}
	}

	int result = 0;
	for (size_t i = start; i < str.length(); ++i)
	{
		const int digit = CharToInt(str[i]);

		if (digit == -1 || digit >= radix)
		{
			wasError = true;
			return 0;
		}

		// Check for overflow when multiplying by radix
		// как можно проще?
		if (negative)
		{
			if (result < (INT_MIN + digit) / radix)
			{
				wasError = true;
				return 0;
			}
			result = result * radix - digit;
		}
		else
		{
			if (result > (INT_MAX - digit) / radix)
			{
				wasError = true;
				return 0;
			}
			result = result * radix + digit;
		}
	}

	return result;
}

std::string IntToString(int inputNumber, const int radix, bool& wasError)
{
	wasError = false;
	if (inputNumber == 0)
	{
		return "0";
	}

	std::string result;
	result.reserve(32); // int >= 2 radix
	bool negative = false;
	unsigned int absValue;
	if (inputNumber < 0)
	{
		negative = true;
		absValue = static_cast<unsigned int>(-(inputNumber + 1)) + 1U;
	}
	else
	{
		absValue = static_cast<unsigned int>(inputNumber);
	}

	while (absValue != 0)
	{
		const int digit = static_cast<int>(absValue % radix);
		result.push_back(IntToChar(digit));
		absValue /= radix;
	}

	if (negative)
	{
		result.push_back('-');
	}

	std::ranges::reverse(result);

	return result;
}

std::optional<Args> ParseArgs(const int argc, char* argv[])
{
	if (argc != 4)
	{
		return std::nullopt;
	}

	// Parse source radix
	Args args;
	bool wasError = false;
	const int sourceRadix = StringToInt(argv[1], 10, wasError);
	if (wasError)
	{
		return std::nullopt;
	}
	args.sourceRadix = sourceRadix;

	// Parse dest radix
	const int destRadix = StringToInt(argv[2], 10, wasError);
	if (wasError)
	{
		return std::nullopt;
	}
	args.destRadix = destRadix;

	args.value = argv[3];
	if (args.sourceRadix < MIN_RADIX
		|| args.sourceRadix > MAX_RADIX
		|| args.destRadix < MIN_RADIX
		|| args.destRadix > MAX_RADIX)
	{
		return std::nullopt;
	}

	return args;
}

int main(int argc, char* argv[])
{
	if (argc == 2 && std::string(argv[1]) == "-h")
	{
		PrintHelp();
		return 0;
	}

	const auto args = ParseArgs(argc, argv);
	if (!args)
	{
		PrintError("Invalid arguments. Usage: radix.exe <source> <dest> <value>");
		return 1;
	}

	bool wasError = false;
	const int number = StringToInt(args->value, args->sourceRadix, wasError);
	if (wasError)
	{
		PrintError("Invalid input value for the given source radix");
		return 1;
	}

	const std::string result = IntToString(number, args->destRadix, wasError);
	if (wasError)
	{
		PrintError("Conversion error during output formatting");
		return 1;
	}

	std::cout << result << "\n";
	return 0;
}