#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "../include/HTMLDecoder.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("HtmlDecode decodes basic entities", "[basic]")
{
	CHECK(HtmlDecode("&lt;") == "<");
	CHECK(HtmlDecode("&gt;") == ">");
	CHECK(HtmlDecode("&amp;") == "&");
	CHECK(HtmlDecode("&quot;") == "\"");
	CHECK(HtmlDecode("&apos;") == "'");
}

TEST_CASE("HtmlDecode handles the task example", "[example]")
{
	const std::string input = R"(Cat &lt;says&gt; &quot;Meow&quot;. M&amp;M&apos;s)";
	const std::string expected = R"(Cat <says> "Meow". M&M's)";

	CHECK(HtmlDecode(input) == expected);
}

TEST_CASE("HtmlDecode handles mixed content", "[mixed]")
{
	CHECK(HtmlDecode("Hello &lt;world&gt;!") == "Hello <world>!");
	CHECK(HtmlDecode("A &amp; B &lt; C") == "A & B < C");
}

TEST_CASE("HtmlDecode ignores unknown entities", "[unknown]")
{
	// Неизвестные сущности должны оставаться неизменными
	CHECK(HtmlDecode("&nbsp;") == "&nbsp;");
	CHECK(HtmlDecode("&unknown;") == "&unknown;");
	CHECK(HtmlDecode("&") == "&");
	CHECK(HtmlDecode("&lt") == "&lt"); // Без точки с запятой
}

TEST_CASE("HtmlDecode handles empty and plain strings", "[edge]")
{
	CHECK(HtmlDecode("") == "");
	CHECK(HtmlDecode("No entities here") == "No entities here");
}

TEST_CASE("HtmlDecode performance check", "[performance]")
{
	std::string input;
	input.reserve(100000);
	for (int i = 0; i < 20000; ++i) {
		input += "Text &lt;>&amp;&quot;&apos; ";
	}

	std::string result = HtmlDecode(input);

	REQUIRE(result.size() < input.size());
	REQUIRE(result.find("&lt;") == std::string::npos);
	REQUIRE(result.find("&amp;") == std::string::npos);
}