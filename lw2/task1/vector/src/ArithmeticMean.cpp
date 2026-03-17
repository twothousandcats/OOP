#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <ranges>
#include <sstream>

bool ReadNumbers(std::vector<double> &numbers, std::istream &input = std::cin) {
    double value;
    input.clear();
    while (input >> value) {
        numbers.push_back(value);
    }

    if (!input.eof()) {
        input.clear();
        return false;
    }
    return true;
}

void ProcessNumbers(std::vector<double> &numbers) {
    if (numbers.empty()) {
        return;
    }

    const double sumPositive = std::accumulate(
        numbers.begin(),
        numbers.end(),
        0.0,
        [](const double acc, const double val) {
            return val > 0
                       ? acc + val
                       : acc;
        }
    );

    const int countPositive = static_cast<int>(
        std::ranges::count_if(
            numbers,
            [](const double val) { return val > 0; }
        )
    );

    double average = 0.0;
    if (countPositive > 0) {
        average = sumPositive / countPositive;
    }
    // todo: как под капотом работает std::ranges
    std::ranges::transform(
        numbers,
        numbers.begin(),
        [average](const double val) { return val + average; }
    );
}

void PrintSortedNumbers(const std::vector<double> &numbers) {
    for (const auto number: numbers) {
        std::cout << number << ' ';
    }
    std::cout << std::endl;
    std::cout << std::flush;
}
