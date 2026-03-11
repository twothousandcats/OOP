#include <iostream>
#include <string>
#include "UrlParser.h"

int main()
{
	std::string inputLine;
	if (!std::getline(std::cin, inputLine))
	{
		std::cout << "ERROR\n";
		return 0;
	}

	if (url::Components components; UrlParser::ParseURL(inputLine, components))
	{
		std::cout << inputLine << '\n';
		std::cout << "HOST: " << components.host << '\n';
		std::cout << "PORT: " << components.port << '\n';
		std::cout << "DOC: " << components.document << '\n';
	}
	else
	{
		std::cout << "ERROR\n";
	}

	return 0;
}