#define CATCH_CONFIG_MAIN
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
    std::istringstream input("- 2 3"); // "-" не является числом в данном контексте потока

    bool result = ReadNumbers(numbers, input);

    REQUIRE(result == false);
    // Числа до ошибки могли считаться, но функция должна вернуть false
}

TEST_CASE("ReadNumbers: Empty input", "[ReadNumbers]") {
    std::vector<double> numbers;
    std::istringstream input("");

    bool result = ReadNumbers(numbers, input);

    REQUIRE(result == true);
    REQUIRE(numbers.empty());
}

TEST_CASE("ReadNumbers: Mixed whitespace", "[ReadNumbers]") {
    std::vector<double> numbers;
    std::istringstream input("1.5\t2.5\n3.5");

    bool result = ReadNumbers(numbers, input);

    REQUIRE(result == true);
    REQUIRE(numbers.size() == 3);
}

TEST_CASE("ProcessNumbers: Example 1 logic", "[ProcessNumbers]") {
    // Вход: 1.0 2 3.659512
    // Положительные: все. Сумма: 6.659512. Кол-во: 3. Среднее: ~2.219837
    // Результат прибавления: 3.2198..., 4.2198..., 5.879...
    std::vector<double> numbers = {1.0, 2.0, 3.659512};

    ProcessNumbers(numbers);

    REQUIRE(numbers.size() == 3);
    // Проверяем с небольшой погрешностью
    REQUIRE(numbers[0] == Catch::Approx(1.0 + 2.219837333).epsilon(0.001));
    REQUIRE(numbers[1] == Catch::Approx(2.0 + 2.219837333).epsilon(0.001));
    REQUIRE(numbers[2] == Catch::Approx(3.659512 + 2.219837333).epsilon(0.001));
}

TEST_CASE("ProcessNumbers: Only negative numbers (Example 3)", "[ProcessNumbers]") {
    // Среднее положительных = 0. Ничего не меняем.
    std::vector<double> numbers = {-1.0004000, -703.0, -3.659512, -11.0};
    std::vector<double> expected = numbers; // Копия для сравнения

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
    // Вход после ProcessNumbers (если среднее было 10): -20, 14, 20, 26
    // Сортировка: -20, 14, 20, 26
    std::vector<double> numbers = {26.0, 14.0, -20.0, 20.0};

    std::string output = GetFormattedSortedOutput(numbers);

    // Ожидаемый формат: "-20.000 14.000 20.000 26.000 " (с пробелом в конце)
    REQUIRE(output == "-20.000 14.000 20.000 26.000 ");
}

TEST_CASE("PrintSortedNumbers: Rounding (Example 1)", "[PrintSortedNumbers]") {
    // Числа после обработки: ~3.219837, ~4.219837, ~5.879349
    // Округление до 3 знаков: 3.220, 4.220, 5.879
    std::vector<double> numbers = {3.219837333, 4.219837333, 5.879349333};

    std::string output = GetFormattedSortedOutput(numbers);

    REQUIRE(output == "3.220 4.220 5.879 ");
}

TEST_CASE("Integration: Full flow Example 4 (Error)", "[Integration]") {
    std::vector<double> numbers;
    std::istringstream input("- 2 3");

    if (!ReadNumbers(numbers, input)) {
        // Эмуляция поведения main
        REQUIRE(true); // Ошибка корректно обнаружена
    } else {
        FAIL("Expected error on invalid input");
    }
}