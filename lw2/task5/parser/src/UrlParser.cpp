#include "UrlParser.h"
#include <algorithm>

const std::regex& UrlParser::GetUrlRegex()
{
	// ports, domain(except :, /), port(opt), any letters(opt)
	static const std::regex urlRegex(
		R"(^(http|https|ftp)://([^:/\s]+)(?::(\d+))?(/.*)?$)",
		std::regex::optimize
		| std::regex::icase
		);
	return urlRegex;
}

bool UrlParser::IsValidPort(const int port)
{
	return port >= 1 && port <= 65535;
}

int UrlParser::GetDefaultPort(const url::Protocol protocol)
{
	switch (protocol)
	{
	case url::Protocol::HTTP:
		return 80;
	case url::Protocol::HTTPS:
		return 443;
	case url::Protocol::FTP:
		return 21;
	}
	return 0;
}

std::optional<url::Protocol> UrlParser::StringToProtocol(const std::string& protoStr)
{
	std::string lowerProto = protoStr;
	std::ranges::transform(
		lowerProto,
		lowerProto.begin(),
		[](const unsigned char ch) { return std::tolower(ch); }
		);

	if (lowerProto == "http")
	{
		return url::Protocol::HTTP;
	}
	if (lowerProto == "https")
	{
		return url::Protocol::HTTPS;
	}
	if (lowerProto == "ftp")
	{
		return url::Protocol::FTP;
	}

	return std::nullopt;
}

bool UrlParser::ExtractProtocol(const std::smatch& match, url::Protocol& outProtocol)
{
	const std::string protoStr = match[1].str();
	const auto protocolOpt = StringToProtocol(protoStr);
	if (!protocolOpt.has_value())
	{
		return false;
	}

	outProtocol = protocolOpt.value();
	return true;
}

bool UrlParser::ExtractHost(const std::smatch& match, std::string& outHost)
{
	const std::string host = match[2].str();

	if (host.empty())
	{
		return false;
	}

	outHost = host;
	return true;
}

bool UrlParser::ExtractPort(const std::smatch& match, const url::Protocol protocol, int& outPort)
{
	if (match[3].matched)
	{
		const std::string portStr = match[3].str();
		try
		{
			const int portVal = std::stoi(portStr);

			if (!IsValidPort(portVal))
			{
				return false;
			}
			outPort = portVal;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}
	else
	{
		outPort = GetDefaultPort(protocol);
	}
	return true;
}

bool UrlParser::ExtractDocument(const std::smatch& match, std::string& outDocument)
{
	if (match[4].matched)
	{
		std::string doc = match[4].str();
		if (!doc.empty() && doc.front() == '/')
		{
			doc.erase(0, 1);
		}
		outDocument = std::move(doc);
	}
	else
	{
		outDocument.clear();
	}
	return true;
}

bool UrlParser::ParseURL(const std::string& url, url::Components& outComponents)
{
	if (url.empty())
	{
		return false;
	}

	const std::regex& regex = GetUrlRegex(); // get regex inst
	std::smatch match;

	if (!std::regex_match(url, match, regex))
	{
		return false;
	}

	// todo: передавать сразу компонент match
	if (!ExtractProtocol(match, outComponents.protocol)
		|| !ExtractHost(match, outComponents.host)
		|| !ExtractPort(match, outComponents.protocol, outComponents.port)
		|| !ExtractDocument(match, outComponents.document))
	{
		return false;
	}

	return true;
}