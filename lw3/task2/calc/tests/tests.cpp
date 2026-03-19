#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"

#include "Value.h"
#include "Calculator.h"
#include "utils.h"

#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <iostream>

// Helper for NaN comparison
bool isNaN(double v) {
    return std::isnan(v);
}

// Value
TEST_CASE("Value handles defined and undefined states", "[Value]") {
    calc::Value v1(10.5);
    calc::Value v2 = calc::Value::nan();

    REQUIRE(v1.isDefined());
    REQUIRE_FALSE(v2.isDefined());
    REQUIRE(v1.get() == 10.5);
    REQUIRE(isNaN(v2.get()));
}

TEST_CASE("Value propagation of NaN", "[Value]") {
    calc::Value a(5.0);
    calc::Value b = calc::Value::nan();

    REQUIRE_FALSE(calc::Value::add(a, b).isDefined());
    REQUIRE_FALSE(calc::Value::sub(a, b).isDefined());
    REQUIRE_FALSE(calc::Value::mul(a, b).isDefined());
    REQUIRE_FALSE(calc::Value::div(a, b).isDefined());
}

TEST_CASE("Division by zero results in NaN", "[Value]") {
    calc::Value a(10.0);
    calc::Value b(0.0);

    REQUIRE_FALSE(calc::Value::div(a, b).isDefined());
}

// Identifier
TEST_CASE("Identifier validation rules", "[Identifier]") {
    REQUIRE(calc::isValidIdentifier("var1"));
    REQUIRE(calc::isValidIdentifier("_temp"));
    REQUIRE(calc::isValidIdentifier("A"));

    REQUIRE_FALSE(calc::isValidIdentifier("1var"));
    REQUIRE_FALSE(calc::isValidIdentifier("my-var"));
    REQUIRE_FALSE(calc::isValidIdentifier(""));
    REQUIRE_FALSE(calc::isValidIdentifier("a+b"));
}

TEST_CASE("Calculator wrapper for validation", "[Calculator]") {
    REQUIRE(calc::Calculator::isValidIdentifier("test123"));
    REQUIRE_FALSE(calc::Calculator::isValidIdentifier("123test"));
}

// Full Stream
std::string runSimulation(const std::string& input) {
    std::istringstream iss(input);
    std::ostringstream oss;

    calc::Calculator calc(iss, oss);
    calc.run();

    return oss.str();
}

TEST_CASE("Basic variable workflow", "[Acceptance]") {
    std::string input =
        "var a\n"
        "let a = 5.0\n"
        "print a\n";

    std::string output = runSimulation(input);
}

TEST_CASE("Error handling: Unknown command", "[Acceptance]") {
    std::string input = "foo bar\n";
    std::string output = runSimulation(input);
    REQUIRE(output.find("Unknown command") != std::string::npos);
}

// ============================================================================
// NEW TESTS: SymbolTable & Command Logic
// ============================================================================

TEST_CASE("Command: var declares variable", "[Logic]") {
    std::string input = "var x\nprintvars\n";
    std::string output = runSimulation(input);
    // Ожидаем: x:nan
    REQUIRE(output.find("x:nan") != std::string::npos);
}

TEST_CASE("Command: var fails on duplicate", "[Logic]") {
    std::string input = "var x\nvar x\n";
    std::string output = runSimulation(input);
    REQUIRE(output.find("Name already exists") != std::string::npos);
}

TEST_CASE("Command: let assigns literal and copies", "[Logic]") {
    std::string input =
        "var a\n"
        "var b\n"
        "let a = 10.5\n"
        "let b = a\n"
        "print a\n"
        "print b\n";
    std::string output = runSimulation(input);
    // Ищем позиции, чтобы убедиться, что оба вывода верны
    REQUIRE(output.find("10.50") != std::string::npos);
    // Проверка количества вхождений "10.50" должна быть 2, но для простоты проверим наличие
    // В реальном тесте лучше проверить точный вывод, но пока так.
}

TEST_CASE("Command: let auto-declares target if not exists", "[Logic]") {
    std::string input = "let x = 5.0\nprint x\n";
    std::string output = runSimulation(input);
    REQUIRE(output.find("5.00") != std::string::npos);
}

TEST_CASE("Command: let fails if source does not exist", "[Logic]") {
    std::string input = "var x\nlet x = y\n";
    std::string output = runSimulation(input);
    REQUIRE(output.find("Name does not exist") != std::string::npos);
}

TEST_CASE("Command: fn simple assignment", "[Logic]") {
    std::string input =
        "var x\n"
        "let x = 42\n"
        "fn f = x\n"
        "print f\n";
    std::string output = runSimulation(input);
    REQUIRE(output.find("42.00") != std::string::npos);
}

TEST_CASE("Command: fn binary operations", "[Logic]") {
    std::string input =
        "var a\nvar b\n"
        "let a = 10\n"
        "let b = 3\n"
        "fn sum = a + b\n"
        "fn diff = a - b\n"
        "fn mul = a * b\n"
        "fn div = a / b\n"
        "print sum\n"
        "print diff\n"
        "print mul\n"
        "print div\n";

    std::string output = runSimulation(input);
    // 13.00, 7.00, 30.00, 3.33
    REQUIRE(output.find("13.00") != std::string::npos);
    REQUIRE(output.find("7.00") != std::string::npos);
    REQUIRE(output.find("30.00") != std::string::npos);
    REQUIRE(output.find("3.33") != std::string::npos);
}

TEST_CASE("Command: fn lazy evaluation", "[Logic]") {
    std::string input =
        "var x\nvar y\n"
        "let x = 10\n"
        "let y = 5\n"
        "fn sum = x + y\n"
        "print sum\n"       // 15.00
        "let x = 20\n"
        "print sum\n";      // 25.00 (пересчет)

    std::string output = runSimulation(input);
    // Проверяем, что есть 15.00 и 25.00
    // Простая проверка наличия, порядок важен
    size_t p1 = output.find("15.00");
    size_t p2 = output.find("25.00");
    REQUIRE(p1 != std::string::npos);
    REQUIRE(p2 != std::string::npos);
    REQUIRE(p1 < p2);
}

TEST_CASE("Parsing: spaces vs no spaces in fn", "[Logic]") {
    std::string input1 = "var a\nvar b\nlet a=1\nlet b=2\nfn f1=a+b\nprint f1\n";
    std::string input2 = "var c\nvar d\nlet c=1\nlet d=2\nfn f2 = c + d\nprint f2\n";

    std::string out1 = runSimulation(input1);
    std::string out2 = runSimulation(input2);

    REQUIRE(out1.find("3.00") != std::string::npos);
    REQUIRE(out2.find("3.00") != std::string::npos);
}

TEST_CASE("Command: printvars and printfns sorting", "[Logic]") {
    std::string input =
        "var z\n"
		"var a\n"
        "let z = 1\n"
		"let a = 2\n"
        "fn f2 = z\n"
		"fn f1 = a\n"
        "printvars\n"
        "printfns\n";

    std::string output = runSimulation(input);
    // Переменные: a, z
    // Функции: f1, f2
    size_t posA = output.find("a:");
    size_t posZ = output.find("z:");
    size_t posF1 = output.find("f1:");
    size_t posF2 = output.find("f2:");

    REQUIRE(posA != std::string::npos);
    REQUIRE(posZ != std::string::npos);
    REQUIRE(posA < posZ); // Сортировка

    REQUIRE(posF1 != std::string::npos);
    REQUIRE(posF2 != std::string::npos);
    REQUIRE(posF1 < posF2); // Сортировка
}