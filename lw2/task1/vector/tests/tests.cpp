#include "catch2/catch_all.hpp"
#include "ArithmeticMean.h"
#include <sstream>
#include <cmath>

TEST_CASE("ReadNumbers: Valid input", "[ReadNumbers]") {
    std::vector<double> numbers;
    std::istringstream input("1.0 2 3.659512");

    bool result = ReadNumbers(numbers, input);

    REQUIRE(result == true);
    REQUIRE(numbers.size() == 3);
    REQUIRE(numbers[0] == Catch::Approx(1.0));
    REQUIRE(numbers[1] == Catch::Approx(2.0));
    REQUIRE(numbers[2] == Catch::Approx(3.659512));
}

TEST_CASE("ReadNumbers: Invalid input (non-numeric)", "[ReadNumbers]") {
    std::vector<double> numbers;
    std::istringstream input("- 2 3");

    REQUIRE(ReadNumbers(numbers, input) == false);
    // Числа до ошибки могли считаться, но функция должна вернуть false
}

TEST_CASE("ReadNumbers: Empty input", "[ReadNumbers]") {
    std::vector<double> numbers;
    std::istringstream input("");

    REQUIRE(ReadNumbers(numbers, input) == true);
    REQUIRE(numbers.empty());
}

TEST_CASE("ReadNumbers: Mixed whitespace", "[ReadNumbers]") {
    std::vector<double> numbers;
    std::istringstream input("1.5\t2.5\n3.5"); // tab & whitespace

    REQUIRE(ReadNumbers(numbers, input) == true);
    REQUIRE(numbers.size() == 3);
}

TEST_CASE("ProcessNumbers: Only negative numbers (Example 3)", "[ProcessNumbers]") {
    // avg = 0
    std::vector<double> numbers = {-1.0004000, -703.0, -3, -11};
    const std::vector<double> expected = numbers;
    ProcessNumbers(numbers);

    REQUIRE(numbers.size() == 4);
    for (size_t i = 0; i < numbers.size(); ++i) {
        REQUIRE(numbers[i] == Catch::Approx(expected[i]));
    }
}

TEST_CASE("ProcessNumbers: Empty vector", "[ProcessNumbers]") {
    std::vector<double> numbers = {};
    ProcessNumbers(numbers);
    REQUIRE(numbers.empty());
}

TEST_CASE("PrintSortedNumbers: Formatting and Sorting (Example 2)", "[PrintSortedNumbers]") {
    const std::vector<double> numbers = {26.0, 14.0, -20.0, 20.0};
    std::string output = GetFormattedSortedOutput(numbers);

    REQUIRE(output == "-20.000 14.000 20.000 26.000 ");
}

TEST_CASE("PrintSortedNumbers: Rounding (Example 1)", "[PrintSortedNumbers]") {
    const std::vector<double> numbers = {3.219837333, 4.219837333, 5.879349333};
    std::string output = GetFormattedSortedOutput(numbers);

    REQUIRE(output == "3.220 4.220 5.879 ");
}

TEST_CASE("Integration: Full flow Example 4 (Error)", "[Integration]") {
	std::istringstream input("- 2 3");

    if (std::vector<double> numbers; !ReadNumbers(numbers, input)) {
        REQUIRE(true);
    } else {
        FAIL("Expected error on invalid input");
    }
}