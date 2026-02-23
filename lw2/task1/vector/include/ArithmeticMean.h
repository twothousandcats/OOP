#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <ranges>
#include <sstream>

inline bool ReadNumbers(std::vector<double>& numbers, std::istream& input = std::cin)
{
	double value;
	input.clear();

	while (input >> value)
	{
		numbers.push_back(value);
	}

	if (!input.eof())
	{
		input.clear();
		return false;
	}
	return true;
}

inline void ProcessNumbers(std::vector<double>& numbers)
{
	if (numbers.empty())
		return;

	const double sumPositive = std::accumulate(numbers.begin(), numbers.end(), 0.0,
		[](const double acc, const double val) {
			return (val > 0) ? acc + val : acc;
		});

	const int countPositive = static_cast<int>(std::ranges::count_if(numbers,
		[](const double val) { return val > 0; }));

	double average = 0.0;
	if (countPositive > 0)
	{
		average = sumPositive / countPositive;
	}

	std::ranges::transform(numbers, numbers.begin(),
		[average](const double val) { return val + average; });
}

// Auxiliary formatting function
inline std::string GetFormattedSortedOutput(const std::vector<double>& numbers)
{
	std::vector<double> sortedNumbers = numbers;
	std::ranges::sort(sortedNumbers);

	std::ostringstream oss;
	oss << std::fixed << std::setprecision(3);
	for (const auto number : sortedNumbers)
	{
		oss << number << ' ';
	}

	return oss.str();
}

inline void PrintSortedNumbers(const std::vector<double>& numbers)
{
	std::cout << GetFormattedSortedOutput(numbers) << std::endl;
}