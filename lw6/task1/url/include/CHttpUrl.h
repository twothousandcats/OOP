#pragma once

#include <string>

enum class Protocol
{
	HTTP,
	HTTPS,
};

class CHttpUrl
{
public:
	explicit CHttpUrl(const std::string& url);

	CHttpUrl(const std::string& domain, const std::string& document, Protocol protocol = Protocol::HTTP);

	CHttpUrl(const std::string& domain, const std::string& document, Protocol protocol, unsigned short port);

	std::string GetURL() const;

	std::string GetDomain() const;

	std::string GetDocument() const;

	Protocol GetProtocol() const;

	unsigned short GetPort() const;

	static unsigned short GetDefaultPort(Protocol protocol) noexcept;

private:
	Protocol m_protocol;
	std::string m_domain;
	std::string m_document;
	unsigned short m_port;
};