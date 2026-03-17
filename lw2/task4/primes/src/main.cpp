#include "PrimeGenerator.h"
#include <iostream>
#include <string>
#include <cstdlib>

void PrintUsage(const char* programName)
{
	std::cerr << "Usage:\n";
	std::cerr << programName << " <upper_bound>\n";
	std::cerr << "  upper_bound: integer value (max 100,000,000)\n";
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		PrintUsage(argv[0]);
		return 1;
	}

	int upperBound;
	try
	{
		upperBound = std::stoi(argv[1]);
	}
	catch (const std::exception&)
	{
		std::cerr << "Error: Invalid integer format.\n";
		PrintUsage(argv[0]);
		return 1;
	}

	try
	{
	    PrimeGenerator generator(upperBound);
		for (const auto& primes = generator.GetPrimes(); const int prime : primes)
		{
			std::cout << prime << "\n";
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}

	return 0;
}