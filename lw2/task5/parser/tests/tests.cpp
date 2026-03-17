#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "UrlParser.h"
#include <chrono>
#include <regex>

// todo: функция для тестов expected : real

TEST_CASE("UrlParser handles basic valid URLs", "[basic]")
{
    url::Components comp;

    SECTION("Simple HTTP")
    {
        REQUIRE(UrlParser::ParseURL("http://example.com", comp));
        CHECK(comp.protocol == url::Protocol::HTTP);
        CHECK(comp.host == "example.com");
        CHECK(comp.port == 80);
        CHECK(comp.document.empty());
    }

    SECTION("Simple HTTPS with default port")
    {
        REQUIRE(UrlParser::ParseURL("https://secure.site.org", comp));
        CHECK(comp.protocol == url::Protocol::HTTPS);
        CHECK(comp.host == "secure.site.org");
        CHECK(comp.port == 443);
        CHECK(comp.document.empty());
    }

    SECTION("Simple FTP")
    {
        REQUIRE(UrlParser::ParseURL("ftp://files.server.net", comp));
        CHECK(comp.protocol == url::Protocol::FTP);
        CHECK(comp.host == "files.server.net");
        CHECK(comp.port == 21);
        CHECK(comp.document.empty());
    }
}

TEST_CASE("UrlParser handles case insensitivity", "[case]")
{
	SECTION("Mixed case protocol")
    {
	    url::Components comp;
	    REQUIRE(UrlParser::ParseURL("HtTp://example.com", comp));
        CHECK(comp.protocol == url::Protocol::HTTP);

        REQUIRE(UrlParser::ParseURL("hTtPs://example.com", comp));
        CHECK(comp.protocol == url::Protocol::HTTPS);

        REQUIRE(UrlParser::ParseURL("Ftp://example.com", comp));
        CHECK(comp.protocol == url::Protocol::FTP);
    }
}

TEST_CASE("UrlParser handles custom ports", "[port]")
{
    url::Components comp;
    SECTION("Valid custom port")
    {
        REQUIRE(UrlParser::ParseURL("http://example.com:8080/", comp));
        CHECK(comp.port == 8080);
    }

    SECTION("Port boundary min (1)")
    {
        REQUIRE(UrlParser::ParseURL("http://example.com:1", comp));
        CHECK(comp.port == 1);
    }

    SECTION("Port boundary max (65535)")
    {
        REQUIRE(UrlParser::ParseURL("http://example.com:65535", comp));
        CHECK(comp.port == 65535);
    }
}

TEST_CASE("UrlParser handles documents and paths", "[doc]")
{
    url::Components comp;

    SECTION("Simple path")
    {
        REQUIRE(UrlParser::ParseURL("http://example.com/index.html", comp));
        CHECK(comp.document == "index.html");
    }

    SECTION("Nested path")
    {
        REQUIRE(UrlParser::ParseURL("http://example.com/a/b/c/file.txt", comp));
        CHECK(comp.document == "a/b/c/file.txt");
    }

    SECTION("Path with query and fragment (Example 4 from task)")
    {
    	// git
        const std::string url = "http://www.mysite.com/docs/document1.html?page=30&lang=en#title";
        REQUIRE(UrlParser::ParseURL(url, comp));
        CHECK(comp.host == "www.mysite.com");
        CHECK(comp.document == "docs/document1.html?page=30&lang=en#title");
    }

    SECTION("Empty document implies root (but output should be empty string per task examples)")
    {
        REQUIRE(UrlParser::ParseURL("http://example.com/", comp));
        CHECK(comp.document.empty());
    }
}

TEST_CASE("UrlParser rejects invalid URLs", "[negative]")
{
    url::Components comp;

    SECTION("Empty string")
    {
        CHECK_FALSE(UrlParser::ParseURL("", comp));
    }

    SECTION("Missing protocol")
    {
        CHECK_FALSE(UrlParser::ParseURL("example.com", comp));
        CHECK_FALSE(UrlParser::ParseURL("//example.com", comp));
    }

    SECTION("Invalid protocol")
    {
        CHECK_FALSE(UrlParser::ParseURL("gopher://example.com", comp));
        CHECK_FALSE(UrlParser::ParseURL("htp://example.com", comp)); // Typo
        CHECK_FALSE(UrlParser::ParseURL("mailto:user@example.com", comp));
    }

    SECTION("Missing host")
    {
        CHECK_FALSE(UrlParser::ParseURL("http://", comp));
        CHECK_FALSE(UrlParser::ParseURL("http://:8080", comp));
    }

    SECTION("Invalid characters in port")
    {
        CHECK_FALSE(UrlParser::ParseURL("http://example.com:abc", comp));
        CHECK_FALSE(UrlParser::ParseURL("http://example.com:8080a", comp));
    }

    SECTION("Spaces are not allowed in host")
    {
        CHECK_FALSE(UrlParser::ParseURL("http://my site.com", comp));
    }

	SECTION("Port zero is invalid")
    {
    	CHECK_FALSE(UrlParser::ParseURL("http://example.com:0", comp));
    }

	SECTION("Port overflow (65536) is invalid")
    {
    	CHECK_FALSE(UrlParser::ParseURL("http://example.com:65536", comp));
    }

	SECTION("Huge port number is invalid")
    {
    	CHECK_FALSE(UrlParser::ParseURL("http://example.com:999999", comp));
    }
}