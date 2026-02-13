#include <algorithm>
#include <cctype>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

constexpr int DEFAULT_ARGUMENTS_COUNT = 4;
constexpr int SOURCE_NOTATION_TYPE_INDEX = 1;
constexpr int DESTINATION_NOTATION_TYPE_INDEX = 2;
constexpr int VALUE_INDEX = 3;

constexpr int MIN_NOTATION = 2;
constexpr int MAX_NOTATION = 36;

constexpr auto* INVALID_NOTATION = "Некорректный ввод системы счисления. "
	"Программа поддерживает нотации со 2 по 36.";
constexpr auto* INVALID_NUMBER_NOTATION = "Недопустимое число для заданной системы счисления источника.";
constexpr auto* INVALID_CONVERSION = "Ошибка конвертации!";
constexpr auto* EXCEPTION_ERROR_TEMPLATE = "Ошибка: ";

constexpr auto* GENERAL_USAGE_INFO = "Используйте терминал для использования программы!";
constexpr auto* USAGE_INFO_WINDOWS = "Чтобы использовать программу на Win OS введите:";
constexpr auto* USAGE_INFO_UNIX = "Чтобы использовать программу на MacOS/Linux введите:";
constexpr auto* WINDOWS_EXECUTION = "radix.exe";
constexpr auto* UNIX_EXECUTION = "./radix";
constexpr auto* ARGUMENTS_SAMPLE = " <source notation> <destination notation> <value>";

// ascii, to MAX_NOTATION(36) - 1
int CharToDigit(const char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		// 0(48)
		return ch - '0';
	}
	if (ch >= 'A' && ch <= 'Z')
	{
		// А(65)
		return ch - 'A' + 10;
	}

	return -1; // error
}

char DigitToChar(const int digit)
{
	if (digit >= 0 && digit <= 9)
	{
		return '0' + digit;
	}
	if (digit >= 10 && digit <= MAX_NOTATION - 1)
	{
		return 'A' + digit - 10;
	}

	return '\0'; // error
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
	bool isNegative = false;
	if (str[0] == '-')
	{
		isNegative = true;
		start = 1;
		if (str.length() == 1)
		{
			wasError = true;
			return 0;
		}
	}
	else if (str[0] == '+')
	{
		start = 1;
		if (str.length() == 1)
		{
			wasError = true;
			return 0;
		}
	}

	// safe modulo accumulation
	unsigned long long result = 0;
	constexpr auto ULL_INT_MAX = static_cast<unsigned long long>(INT_MAX);
	constexpr unsigned long long ULL_INT_MIN_ABS = ULL_INT_MAX + 1ULL; // |INT_MIN|

	for (size_t i = start; i < str.length(); ++i)
	{
		char ch = str[i];
		int digit = CharToDigit(ch);
		if (digit == -1 || digit >= radix)
		{
			wasError = true;
			return 0;
		}

		// Проверка переполнения: даже для отрицательных — модуль не должен превысить |INT_MIN|
		if (result > (ULL_INT_MIN_ABS - digit) / radix)
		{
			wasError = true;
			return 0;
		}
		result = result * radix + digit;
	}

	if (isNegative)
	{
		if (result > ULL_INT_MIN_ABS)
		{
			wasError = true;
			return 0;
		}
		if (result == ULL_INT_MIN_ABS)
		{
			return INT_MIN;
		}
		return -static_cast<int>(result);
	}
	if (result > ULL_INT_MAX)
	{
		wasError = true;
		return 0;
	}
	return static_cast<int>(result);
}

std::string IntToString(const int number, const int radix, bool& wasError)
{
	wasError = false;

	if (number == 0)
	{
		return "0";
	}

	bool isNegative = false;
	unsigned int num;
	if (number == INT_MIN)
	{
		num = static_cast<unsigned int>(INT_MIN);
		isNegative = true;
	}
	else if (number < 0)
	{
		num = -static_cast<unsigned int>(number);
		isNegative = true;
	}
	else
	{
		num = number;
	}

	std::string result;
	while (num > 0)
	{
		const int digit = num % radix;
		result = DigitToChar(digit) + result;
		num = num / radix;
	}

	if (isNegative)
	{
		result = "-" + result;
	}

	return result;
}

int main(int argCount, char* argValues[])
{
	if (argCount != DEFAULT_ARGUMENTS_COUNT)
	{
		std::cerr << GENERAL_USAGE_INFO << std::endl;
		std::cerr << USAGE_INFO_WINDOWS << std::endl;
		std::cerr << WINDOWS_EXECUTION << ARGUMENTS_SAMPLE << std::endl;
		std::cerr << USAGE_INFO_UNIX << std::endl;
		std::cerr << UNIX_EXECUTION << ARGUMENTS_SAMPLE << std::endl;

		return 1;
	}

	try
	{
		const int sourceRadix = std::stoi(argValues[SOURCE_NOTATION_TYPE_INDEX]);
		const int destRadix = std::stoi(argValues[DESTINATION_NOTATION_TYPE_INDEX]);
		const std::string value = argValues[VALUE_INDEX];

		if (sourceRadix < MIN_NOTATION || sourceRadix > MAX_NOTATION || destRadix < MIN_NOTATION
			|| destRadix > MAX_NOTATION)
		{
			std::cerr << INVALID_NOTATION << std::endl;
			return 1;
		}

		bool wasError = false;
		const int number = StringToInt(value, sourceRadix, wasError);

		if (wasError)
		{
			std::cerr << INVALID_NUMBER_NOTATION << std::endl;

			return 1;
		}

		const std::string result = IntToString(number, destRadix, wasError);

		if (wasError)
		{
			std::cerr << INVALID_CONVERSION << std::endl;

			return 1;
		}

		std::cout << result << std::endl;

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << EXCEPTION_ERROR_TEMPLATE << e.what() << std::endl;

		return 1;
	}
}