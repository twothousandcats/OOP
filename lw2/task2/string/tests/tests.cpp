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

TEST_CASE("HtmlDecode ignores unknown entities", "[unknown]")
{
	// unknown -> unknown
	CHECK(HtmlDecode("&nbsp;") == "&nbsp;");
	CHECK(HtmlDecode("&unknown;") == "&unknown;");
	CHECK(HtmlDecode("&") == "&");
	CHECK(HtmlDecode("&lt") == "&lt"); // without ;
}

TEST_CASE("HtmlDecode handles empty and plain strings", "[edge]")
{
	CHECK(HtmlDecode("").empty());
	CHECK(HtmlDecode("No entities here") == "No entities here");
}
TEST_CASE("ProcessHtmlStream processes multiple lines correctly", "[stream]")
{
	std::istringstream input("Line 1 &lt;br&gt;\nLine 2 &amp;&amp;\nNo entities");
	std::ostringstream output;

	ProcessHtmlStream(input, output);

	std::string result = output.str();
	CHECK(result == "Line 1 <br>\nLine 2 &&\nNo entities\n");
}