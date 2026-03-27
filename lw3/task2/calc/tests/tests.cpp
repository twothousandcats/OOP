#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "Calculator.h"
#include "utils.h"
#include "Value.h"
#include <cmath>
#include <iostream>
#include <string>

// Helper
bool isNaN(const double v) { return std::isnan(v); }

TEST_CASE("Value handles defined and undefined states", "[Value]")
{
	const calc::Value v1(10.5);
	const calc::Value v2 = calc::Value::nan();
	REQUIRE(v1.isDefined());
	REQUIRE_FALSE(v2.isDefined());
	REQUIRE(v1.get() == 10.5);
	REQUIRE(std::isnan(v2.get()));
}

TEST_CASE("Division by zero results in NaN", "[Value]")
{
	const calc::Value a(10.0);
	const calc::Value b(0.0);
	REQUIRE_FALSE(calc::Value::div(a, b).isDefined());
}

TEST_CASE("Identifier validation", "[Utils]")
{
	REQUIRE(calc::isValidIdentifier("var1"));
	REQUIRE(calc::isValidIdentifier("_temp"));
	REQUIRE_FALSE(calc::isValidIdentifier("1var"));
	REQUIRE_FALSE(calc::isValidIdentifier(""));
}

// Тестирование через процесс команд
TEST_CASE("Command: var declares variable", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.processCommand("var x").empty());
	REQUIRE(calc.processCommand("printvars").find("x:nan") != std::string::npos);
}

TEST_CASE("Command: var duplicate error", "[Logic]")
{
	calc::Calculator calc;
	calc.processCommand("var x");
	REQUIRE(calc.processCommand("var x") == "Name already exists\n");
}

TEST_CASE("Command: let with literal and copy", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.processCommand("var a").empty());
	REQUIRE(calc.processCommand("let a = 10.5").empty());
	REQUIRE(calc.processCommand("let b = a").empty());

	REQUIRE(calc.processCommand("print a") == "10.50\n");
	REQUIRE(calc.processCommand("print b") == "10.50\n");
}

TEST_CASE("Command: let auto-declare", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.processCommand("let x = 5.0").empty());
	REQUIRE(calc.processCommand("print x") == "5.00\n");
}

TEST_CASE("Command: fn lazy evaluation", "[Logic]")
{
	calc::Calculator calc;
	calc.processCommand("var x");
	calc.processCommand("var y");
	calc.processCommand("let x = 10");
	calc.processCommand("let y = 5");
	calc.processCommand("fn sum = x + y");

	REQUIRE(calc.processCommand("print sum") == "15.00\n");

	calc.processCommand("let x = 20");
	// Значение должно пересчитаться
	REQUIRE(calc.processCommand("print sum") == "25.00\n");
}

TEST_CASE("Command: Unknown command", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.processCommand("foo bar") == "Unknown command\n");
}

TEST_CASE("Command: Invalid usage syntax", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.processCommand("var") == "Invalid usage\n");
	REQUIRE(calc.processCommand("let x") == "Invalid usage\n");
	REQUIRE(calc.processCommand("print nonexistent") == "Name does not exist\n");
}

TEST_CASE("Command: Fibonacci 50th number with lazy recalculation", "[Integration][Performance]")
{
	calc::Calculator calc;

	// base
	REQUIRE(calc.processCommand("let v0=0").empty());
	REQUIRE(calc.processCommand("let v1=1").empty());

	// init
	REQUIRE(calc.processCommand("fn fib0=v0").empty());
	REQUIRE(calc.processCommand("fn fib1=v1").empty());

	// generate until 50
	// Fn(n) = Fn(n-1) + Fn(n-2)
	for (int i = 2; i <= 50; ++i) {
		std::string cmd = "fn fib" + std::to_string(i) + "=fib" + std::to_string(i-1) + "+fib" + std::to_string(i-2);
		REQUIRE(calc.processCommand(cmd).empty());
	}

	std::string out1 = calc.processCommand("print fib50");
	REQUIRE(out1.find("12586269025.00") != std::string::npos);

	REQUIRE(calc.processCommand("let v0=1").empty());
	REQUIRE(calc.processCommand("let v1=1").empty());

	std::string out2 = calc.processCommand("print fib50");
	REQUIRE(out2.find("20365011074.00") != std::string::npos);
}