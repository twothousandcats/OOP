#include "CHttpUrl.h"
#include "CUrlParsingError.h"

#include <iostream>
#include <string>

namespace
{

std::string ProtocolToString(const Protocol protocol)
{
	return (protocol == Protocol::HTTPS) ? "https" : "http";
}

void PrintUrlInfo(std::ostream& out, const CHttpUrl& url)
{
	out << "URL:      " << url.GetURL() << "\n";
	out << "Protocol: " << ProtocolToString(url.GetProtocol()) << "\n";
	out << "Domain:   " << url.GetDomain() << "\n";
	out << "Port:     " << url.GetPort() << "\n";
	out << "Document: " << url.GetDocument() << "\n";
}

void ProcessLine(const std::string& line, std::ostream& out)
{
	if (line.empty())
	{
		return;
	}
	try
	{
		const CHttpUrl url(line);
		PrintUrlInfo(out, url);
	}
	catch (const CUrlParsingError& ex)
	{
		out << "Error: " << ex.what() << "\n";
	}
}

} // namespace

int main()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		ProcessLine(line, std::cout);
	}
	return 0;
}