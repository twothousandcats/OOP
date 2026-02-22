#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <cmath>

bool ReadNumbers(std::vector<double>& numbers)
{
	double value;
	while (std::cin >> value)
	{
		numbers.push_back(value);
	}

	if (!std::cin.eof())
	{
		std::cin.clear();
		return false;
	}
	return true;
}

void ProcessNumbers(std::vector<double>& numbers)
{
	if (numbers.empty())
	{
		return;
	}

	// Вычисляем сумму положительных элементов
	double sumPositive = std::accumulate(numbers.begin(), numbers.end(), 0.0,
		[](double acc, double val) {
			return (val > 0) ? acc + val : acc;
		});

	// Подсчитываем количество положительных элементов
	int countPositive = static_cast<int>(std::ranges::count_if(numbers,
		[](double val) { return val > 0; }));

	double average = 0.0;
	if (countPositive > 0)
	{
		average = sumPositive / countPositive;
	}

	// Прибавляем среднее ко всем элементам
	std::ranges::transform(numbers, numbers.begin(),
		[average](double val) { return val + average; });
}

void PrintSortedNumbers(const std::vector<double>& numbers)
{
	std::vector<double> sortedNumbers = numbers;
	std::ranges::sort(sortedNumbers);

	std::cout << std::fixed << std::setprecision(3);
	for (const auto number : sortedNumbers)
	{
		std::cout << number << ' ';
	}
	std::cout << std::endl;
}

int main()
{
	std::vector<double> numbers;
	if (!ReadNumbers(numbers))
	{
		std::cout << "ERROR" << std::endl;
		return 0;
	}

	ProcessNumbers(numbers);
	PrintSortedNumbers(numbers);

	return 0;
}