#pragma once

#include <string>
#include <optional>
#include <regex>

// group data
namespace url
{
enum class Protocol
{
	HTTP,
	HTTPS,
	FTP
};

struct Components
{
	Protocol protocol;
	int port;
	std::string host;
	std::string document;
};
}

// todo: ParseURL ret Components
// нет членов класса, такие классы не имеют особой необходимости (не нужно поддерживать инвариант членов класса)
class UrlParser
{
public:
	UrlParser() = delete; // static methods, prohibit the creation of instances

	static bool ParseURL(const std::string& url, url::Components& outComponents);

private:
	// Parse stages
	[[nodiscard]] static bool ExtractProtocol(const std::smatch& match, url::Protocol& outProtocol);

	[[nodiscard]] static bool ExtractHost(const std::smatch& match, std::string& outHost);

	[[nodiscard]] static bool ExtractPort(const std::smatch& match, url::Protocol protocol, int& outPort);

	[[nodiscard]] static bool ExtractDocument(const std::smatch& match, std::string& outDocument);

	// Utils
	[[nodiscard]] static std::optional<url::Protocol> StringToProtocol(const std::string& protoStr);

	[[nodiscard]] static int GetDefaultPort(url::Protocol protocol);

	[[nodiscard]] static const std::regex& GetUrlRegex();

	[[nodiscard]] static bool IsValidPort(int port);
};