#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// known entities map
const std::unordered_map<std::string_view, char> htmlEntities = {
	{ "quot", '"' },
	{ "apos", '\'' },
	{ "lt", '<' },
	{ "gt", '>' },
	{ "amp", '&' }
};

// returns decoded entity length (& and ; included)
size_t DecodeEntity(const std::string& html, size_t pos, char& outChar)
{
	if (pos >= html.size() || html[pos] != '&')
	{
		return 0;
	}

	// looking for ;
	const size_t semiPos = html.find(';', pos + 1);
	if (semiPos == std::string::npos)
	{
		return 0;
	}

	// extract entity name
	const std::string_view name = std::string_view(html).substr(pos + 1, semiPos - pos - 1);
	if (const auto it = htmlEntities.find(name); it != htmlEntities.end())
	{
		outChar = it->second;
		return semiPos - pos + 1; // &name;
	}

	return 0; // unknown entity
}

std::string HtmlDecode(const std::string& html)
{
	std::string result;
	result.reserve(html.size()); // opt

	size_t i = 0;
	while (i < html.size())
	{
		if (html[i] == '&')
		{
			char decodedChar;
			if (const size_t consumed = DecodeEntity(html, i, decodedChar); consumed > 0)
			{
				result += decodedChar;
				i += consumed;
			}
			else
			{
				// unknown entity, copy '&'
				result += html[i];
				++i;
			}
		}
		else
		{
			result += html[i];
			++i;
		}
	}
	return result;
}

int main()
{
	// Optimizing I/O for large data flows
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string line;
	while (std::getline(std::cin, line))
	{
		std::cout << HtmlDecode(line) << "\n";
	}

	return 0;
}