#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "Calculator.h"
#include "utils.h"
#include "Value.h"
#include <cmath>
#include <iostream>
#include <string>

// values
TEST_CASE("Value handles defined and undefined states", "[Value]")
{
	const calc::Value v1(10.5);
	const calc::Value v2 = calc::Value::Nan();
	REQUIRE(v1.IsDefined());
	REQUIRE_FALSE(v2.IsDefined());
	REQUIRE(v1.Get() == 10.5);
	REQUIRE(std::isnan(v2.Get()));
}

TEST_CASE("Division by zero results in NaN", "[Value]")
{
	const calc::Value a(10.0);
	const calc::Value b(0.0);
	REQUIRE_FALSE(calc::Value::Div(a, b).IsDefined());
}

TEST_CASE("Identifier validation", "[Utils]")
{
	REQUIRE(calc::isValidIdentifier("var1"));
	REQUIRE(calc::isValidIdentifier("_temp"));
	REQUIRE_FALSE(calc::isValidIdentifier("1var"));
	REQUIRE_FALSE(calc::isValidIdentifier(""));
}

// prog
TEST_CASE("Command: var declares variable", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.ProcessCommand("var x").empty());
	REQUIRE(calc.ProcessCommand("printvars").find("x:nan") != std::string::npos);
}

TEST_CASE("Command: var duplicate error", "[Logic]")
{
	calc::Calculator calc;
	calc.ProcessCommand("var x");
	REQUIRE(calc.ProcessCommand("var x") == "Name already exists\n");
}

TEST_CASE("Command: let with literal and copy", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.ProcessCommand("var a").empty());
	REQUIRE(calc.ProcessCommand("let a = 10.5").empty());
	REQUIRE(calc.ProcessCommand("let b = a").empty());

	REQUIRE(calc.ProcessCommand("print a") == "10.50\n");
	REQUIRE(calc.ProcessCommand("print b") == "10.50\n");
}

TEST_CASE("Command: let auto-declare", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.ProcessCommand("let x = 5.0").empty());
	REQUIRE(calc.ProcessCommand("print x") == "5.00\n");
}

TEST_CASE("Command: fn lazy evaluation", "[Logic]")
{
	calc::Calculator calc;
	calc.ProcessCommand("var x");
	calc.ProcessCommand("var y");
	calc.ProcessCommand("let x = 10");
	calc.ProcessCommand("let y = 5");
	calc.ProcessCommand("fn sum = x + y");

	REQUIRE(calc.ProcessCommand("print sum") == "15.00\n");

	calc.ProcessCommand("let x = 20");
	// Значение должно пересчитаться
	REQUIRE(calc.ProcessCommand("print sum") == "25.00\n");
}

TEST_CASE("Command: Unknown command", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.ProcessCommand("foo bar") == "Unknown command\n");
}

TEST_CASE("Command: Invalid usage syntax", "[Logic]")
{
	calc::Calculator calc;
	REQUIRE(calc.ProcessCommand("var") == "Invalid usage\n");
	REQUIRE(calc.ProcessCommand("let x") == "Invalid usage\n");
	REQUIRE(calc.ProcessCommand("print nonexistent") == "Name does not exist\n");
}

TEST_CASE("Command: Fibonacci numbers", "[Integration][Performance]")
{
	calc::Calculator calc;

	// init base
	REQUIRE(calc.ProcessCommand("let v0=0").empty());
	REQUIRE(calc.ProcessCommand("let v1=1").empty());
	REQUIRE(calc.ProcessCommand("fn fib0=v0").empty());
	REQUIRE(calc.ProcessCommand("fn fib1=v1").empty());

	// Fn(n)=Fn(n-1)+Fn(n-2)
	for (int i = 2; i <= 50; ++i) {
		std::string cmd = "fn fib" + std::to_string(i) + "=fib" + std::to_string(i-1) + "+fib" + std::to_string(i-2);
		REQUIRE(calc.ProcessCommand(cmd).empty());
	}

	std::string out1 = calc.ProcessCommand("print fib50");
	REQUIRE(out1.find("12586269025.00") != std::string::npos);

	REQUIRE(calc.ProcessCommand("let v0=1").empty());
	REQUIRE(calc.ProcessCommand("let v1=1").empty());

	std::string out2 = calc.ProcessCommand("print fib50");
	REQUIRE(out2.find("20365011074.00") != std::string::npos);
}