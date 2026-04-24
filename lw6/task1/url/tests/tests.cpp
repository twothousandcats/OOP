#include <catch2/catch_all.hpp>

#include "CHttpUrl.h"
#include "CUrlParsingError.h"

#include <stdexcept>
#include <string>

TEST_CASE("CHttpUrl parses a minimal http URL")
{
	const CHttpUrl url("http://example.com");
	CHECK(url.GetProtocol() == Protocol::HTTP);
	CHECK(url.GetDomain() == "example.com");
	CHECK(url.GetDocument() == "/");
	CHECK(url.GetPort() == 80);
}

TEST_CASE("CHttpUrl parses an https URL and uses port 443 by default")
{
	const CHttpUrl url("https://example.com/");
	CHECK(url.GetProtocol() == Protocol::HTTPS);
	CHECK(url.GetPort() == 443);
	CHECK(url.GetDocument() == "/");
}

TEST_CASE("CHttpUrl parses document path with nested segments")
{
	const CHttpUrl url("http://example.com/images/photo.jpg");
	CHECK(url.GetDocument() == "/images/photo.jpg");
}

TEST_CASE("CHttpUrl parses custom port")
{
	const CHttpUrl url("http://example.com:8080/index.html");
	CHECK(url.GetPort() == 8080);
	CHECK(url.GetDocument() == "/index.html");
}

TEST_CASE("CHttpUrl protocol parsing is case-insensitive")
{
	CHECK(CHttpUrl("HtTp://example.com").GetProtocol() == Protocol::HTTP);
	CHECK(CHttpUrl("HTTPS://example.com").GetProtocol() == Protocol::HTTPS);
	CHECK(CHttpUrl("HtTps://example.com").GetProtocol() == Protocol::HTTPS);
}

TEST_CASE("CHttpUrl keeps domain unchanged (not lowercased)")
{
	const CHttpUrl url("http://Example.COM/");
	CHECK(url.GetDomain() == "Example.COM");
}

TEST_CASE("CHttpUrl accepts query string and fragment inside document part")
{
	const CHttpUrl url("http://example.com/search?q=cats&page=2#top");
	CHECK(url.GetDocument() == "/search?q=cats&page=2#top");
}

TEST_CASE("CHttpUrl accepts boundary port values")
{
	CHECK(CHttpUrl("http://example.com:1/").GetPort() == 1);
	CHECK(CHttpUrl("http://example.com:65535/").GetPort() == 65535);
}

TEST_CASE("CHttpUrl rejects malformed URL strings")
{
	CHECK_THROWS_AS(CHttpUrl(""), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("example.com"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http:/example.com"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http//example.com"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("://example.com"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://example.com with spaces"), CUrlParsingError);
}

TEST_CASE("CHttpUrl rejects unsupported protocols")
{
	CHECK_THROWS_AS(CHttpUrl("ftp://example.com"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("file://example.com"), CUrlParsingError);
}

TEST_CASE("CHttpUrl rejects empty domain")
{
	CHECK_THROWS_AS(CHttpUrl("http:///index.html"), CUrlParsingError);
}

TEST_CASE("CHttpUrl rejects invalid domain labels")
{
	CHECK_THROWS_AS(CHttpUrl("http://-bad.com/"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://bad-.com/"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://bad..com/"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://bad_domain.com/"), CUrlParsingError);
}

TEST_CASE("CHttpUrl rejects out-of-range or non-numeric ports")
{
	CHECK_THROWS_AS(CHttpUrl("http://example.com:0/"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://example.com:65536/"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://example.com:abc/"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://example.com:/"), CUrlParsingError);
	CHECK_THROWS_AS(CHttpUrl("http://example.com:99999999999999999999/"), CUrlParsingError);
}

TEST_CASE("CUrlParsingError is derived from std::invalid_argument")
{
	try
	{
		CHttpUrl("not a url");
		FAIL("Exception was not thrown");
	}
	catch (const std::invalid_argument& ex)
	{
		SUCCEED("Caught as std::invalid_argument: " << ex.what());
	}
}

TEST_CASE("CHttpUrl component constructor uses default port for protocol")
{
	const CHttpUrl http("example.com", "/index.html", Protocol::HTTP);
	CHECK(http.GetPort() == 80);

	const CHttpUrl https("example.com", "/index.html", Protocol::HTTPS);
	CHECK(https.GetPort() == 443);
}

TEST_CASE("CHttpUrl component constructor prepends slash to document")
{
	const CHttpUrl url("example.com", "index.html", Protocol::HTTP);
	CHECK(url.GetDocument() == "/index.html");
}

TEST_CASE("CHttpUrl component constructor treats empty document as root")
{
	const CHttpUrl url("example.com", "", Protocol::HTTP);
	CHECK(url.GetDocument() == "/");
}

TEST_CASE("CHttpUrl component constructor keeps existing leading slash")
{
	const CHttpUrl url("example.com", "/path/page", Protocol::HTTP);
	CHECK(url.GetDocument() == "/path/page");
}

TEST_CASE("CHttpUrl component constructor accepts custom port")
{
	const CHttpUrl url("example.com", "/", Protocol::HTTP, 8080);
	CHECK(url.GetPort() == 8080);
}

TEST_CASE("CHttpUrl component constructor rejects invalid arguments with std::invalid_argument")
{
	CHECK_THROWS_AS(CHttpUrl("", "/index.html", Protocol::HTTP), std::invalid_argument);
	CHECK_THROWS_AS(
		CHttpUrl("bad domain.com", "/index.html", Protocol::HTTP),
		std::invalid_argument);
	CHECK_THROWS_AS(CHttpUrl("example.com", "/bad path", Protocol::HTTP), std::invalid_argument);
	CHECK_THROWS_AS(CHttpUrl("example.com", "/", Protocol::HTTP, 0), std::invalid_argument);
}

TEST_CASE("CHttpUrl::GetURL omits default port for its protocol")
{
	CHECK(CHttpUrl("http://example.com/").GetURL() == "http://example.com/");
	CHECK(CHttpUrl("http://example.com:80/").GetURL() == "http://example.com/");
	CHECK(CHttpUrl("https://example.com/").GetURL() == "https://example.com/");
	CHECK(CHttpUrl("https://example.com:443/").GetURL() == "https://example.com/");
}

TEST_CASE("CHttpUrl::GetURL includes non-default port")
{
	CHECK(
		CHttpUrl("http://example.com:8080/index.html").GetURL()
		== "http://example.com:8080/index.html");
	CHECK(
		CHttpUrl("https://example.com:8443/").GetURL() == "https://example.com:8443/");
	CHECK(CHttpUrl("http://example.com:443/").GetURL() == "http://example.com:443/");
	CHECK(CHttpUrl("https://example.com:80/").GetURL() == "https://example.com:80/");
}

TEST_CASE("CHttpUrl round-trip: GetURL output can be re-parsed")
{
	const CHttpUrl original("example.com", "images/photo.jpg", Protocol::HTTPS, 8443);
	const CHttpUrl reparsed(original.GetURL());
	CHECK(reparsed.GetProtocol() == original.GetProtocol());
	CHECK(reparsed.GetDomain() == original.GetDomain());
	CHECK(reparsed.GetDocument() == original.GetDocument());
	CHECK(reparsed.GetPort() == original.GetPort());
}