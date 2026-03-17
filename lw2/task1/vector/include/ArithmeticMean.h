#pragma once

#include <vector>
#include <iostream>

bool ReadNumbers(std::vector<double>& numbers, std::istream& input = std::cin);

void ProcessNumbers(std::vector<double>& numbers);

// Auxiliary formatting function
std::string GetFormattedSortedOutput(const std::vector<double>& numbers);

// is that a bit faster to write from prepared string?
void PrintSortedNumbers(const std::vector<double>& numbers);