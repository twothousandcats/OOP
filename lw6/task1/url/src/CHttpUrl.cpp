#include "CHttpUrl.h"
#include "CUrlParsingError.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <regex>
#include <stdexcept>
#include <string>

namespace
{

constexpr unsigned short HTTP_DEFAULT_PORT = 80;
constexpr unsigned short HTTPS_DEFAULT_PORT = 443;
constexpr unsigned int MIN_PORT = 1;
constexpr unsigned int MAX_PORT = 65535;
constexpr std::size_t MAX_DOMAIN_LABEL_LENGTH = 63;

std::string ToLower(const std::string& source)
{
	std::string result = source;
	std::ranges::transform(result, result.begin(),
		[](const unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
	return result;
}

Protocol ParseProtocolString(const std::string& protocolStr)
{
	const std::string lower = ToLower(protocolStr);
	if (lower == "http")
	{
		return Protocol::HTTP;
	}
	if (lower == "https")
	{
		return Protocol::HTTPS;
	}
	throw CUrlParsingError("Unsupported protocol: " + protocolStr);
}

bool IsValidDomainLabel(const std::string& label) noexcept
{
	if (label.empty() || label.size() > MAX_DOMAIN_LABEL_LENGTH)
	{
		return false;
	}
	if (label.front() == '-' || label.back() == '-')
	{
		return false;
	}
	for (const char ch : label)
	{
		const unsigned char uch = static_cast<unsigned char>(ch);
		if (!std::isalnum(uch) && ch != '-')
		{
			return false;
		}
	}
	return true;
}

bool IsValidDomain(const std::string& domain) noexcept
{
	if (domain.empty())
	{
		return false;
	}

	std::size_t start = 0;
	while (start <= domain.size())
	{
		const std::size_t dotPos = domain.find('.', start);
		const std::size_t end = (dotPos == std::string::npos) ? domain.size() : dotPos;
		const std::string label = domain.substr(start, end - start);
		if (!IsValidDomainLabel(label))
		{
			return false;
		}
		if (dotPos == std::string::npos)
		{
			break;
		}
		start = dotPos + 1;
	}
	return true;
}

bool IsValidDocument(const std::string& document) noexcept
{
	if (document.empty() || document.front() != '/')
	{
		return false;
	}
	for (const char ch : document)
	{
		const unsigned char uch = static_cast<unsigned char>(ch);
		if (std::iscntrl(uch) || ch == ' ')
		{
			return false;
		}
	}
	return true;
}

std::string NormalizeDocument(const std::string& document)
{
	if (document.empty())
	{
		return "/";
	}
	if (document.front() != '/')
	{
		return "/" + document;
	}
	return document;
}

unsigned short ParsePort(const std::string& portStr)
{
	if (portStr.empty())
	{
		throw CUrlParsingError("Port is empty");
	}
	for (const char ch : portStr)
	{
		if (!std::isdigit(static_cast<unsigned char>(ch)))
		{
			throw CUrlParsingError("Port must contain only digits: " + portStr);
		}
	}

	unsigned long value = 0;
	try
	{
		value = std::stoul(portStr);
	}
	catch (const std::out_of_range&)
	{
		throw CUrlParsingError("Port value is out of range: " + portStr);
	}

	if (value < MIN_PORT || value > MAX_PORT)
	{
		throw CUrlParsingError("Port must be in range [1, 65535]: " + portStr);
	}
	return static_cast<unsigned short>(value);
}

} // namespace

unsigned short CHttpUrl::GetDefaultPort(const Protocol protocol) noexcept
{
	return (protocol == Protocol::HTTPS)
		? HTTPS_DEFAULT_PORT
		: HTTP_DEFAULT_PORT;
}

CHttpUrl::CHttpUrl(const std::string& url)
	: m_protocol(Protocol::HTTP)
	  , m_port(HTTP_DEFAULT_PORT)
{
	static const std::regex urlRegex(
		R"(^([A-Za-z]+)://([^/:\s]+)(?::([^/\s]*))?(/[^\s]*)?$)");

	std::smatch match;
	if (!std::regex_match(url, match, urlRegex))
	{
		throw CUrlParsingError("Malformed URL: " + url);
	}

	m_protocol = ParseProtocolString(match[1].str());

	const std::string domain = match[2].str();
	if (!IsValidDomain(domain))
	{
		throw CUrlParsingError("Invalid domain: " + domain);
	}
	m_domain = domain;

	if (match[3].matched)
	{
		m_port = ParsePort(match[3].str());
	}
	else
	{
		m_port = GetDefaultPort(m_protocol);
	}

	const std::string document = match[4].matched ? match[4].str() : "/";
	if (!IsValidDocument(document))
	{
		throw CUrlParsingError("Invalid document: " + document);
	}
	m_document = document;
}

CHttpUrl::CHttpUrl(
	const std::string& domain,
	const std::string& document,
	const Protocol protocol,
	const unsigned short port)
	: m_protocol(protocol)
	  , m_port(port)
{
	if (!IsValidDomain(domain))
	{
		throw std::invalid_argument("Invalid domain: " + domain);
	}
	if (port < MIN_PORT)
	{
		throw std::invalid_argument("Port must be in range [1, 65535]");
	}
	const std::string normalized = NormalizeDocument(document);
	if (!IsValidDocument(normalized))
	{
		throw std::invalid_argument("Invalid document: " + document);
	}

	m_domain = domain;
	m_document = normalized;
}

CHttpUrl::CHttpUrl(
	const std::string& domain,
	const std::string& document,
	const Protocol protocol)
	: CHttpUrl(domain, document, protocol, GetDefaultPort(protocol))
{
}

std::string CHttpUrl::GetURL() const
{
	std::string result;
	result += (m_protocol == Protocol::HTTPS) ? "https://" : "http://";
	result += m_domain;
	if (m_port != GetDefaultPort(m_protocol))
	{
		result += ":" + std::to_string(m_port);
	}
	result += m_document;
	return result;
}

std::string CHttpUrl::GetDomain() const
{
	return m_domain;
}

std::string CHttpUrl::GetDocument() const
{
	return m_document;
}

Protocol CHttpUrl::GetProtocol() const
{
	return m_protocol;
}

unsigned short CHttpUrl::GetPort() const
{
	return m_port;
}