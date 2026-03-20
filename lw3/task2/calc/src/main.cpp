#include "Calculator.h"
#include <iostream>
#include <string>

int main()
{
	calc::Calculator calculator;
	std::string line;

	while (std::getline(std::cin, line))
	{
		if (std::string result = calculator.processCommand(line); !result.empty())
		{
			std::cout << result;
		}
	}
	return 0;
}