#include <iostream>
#include <string>
#include "UrlParser.h"

void PrintError()
{
	std::cout << "ERROR\n" << std::endl;
}

int main()
{
	std::string inputLine;
	if (!std::getline(std::cin, inputLine))
	{
		PrintError();
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
		PrintError();
	}

	return 0;
}