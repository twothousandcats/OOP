#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>

// known entities
inline const std::unordered_map<std::string_view, char> htmlEntities = {
	{ "quot", '"' },
	{ "apos", '\'' },
	{ "lt", '<' },
	{ "gt", '>' },
	{ "amp", '&' }
};

inline size_t DecodeEntity(const std::string& html, size_t pos, char& outChar)
{
	if (pos >= html.size() || html[pos] != '&')
	{
		return 0;
	}

	const size_t semiPos = html.find(';', pos + 1);
	if (semiPos == std::string::npos)
	{
		return 0;
	}

	// string_view for searching and copy string
	const std::string_view name = std::string_view(html).substr(pos + 1, semiPos - pos - 1);

	if (const auto it = htmlEntities.find(name); it != htmlEntities.end())
	{
		outChar = it->second;
		return semiPos - pos + 1;
	}

	return 0;
}

inline std::string HtmlDecode(const std::string& html)
{
	std::string result;
	result.reserve(html.size());

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
				// unknows entity
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

inline void ProcessHtmlStream(std::istream& input = std::cin, std::ostream& output = std::cout)
{
	// I/O optimization
	std::ios_base::sync_with_stdio(false);
	input.tie(nullptr);

	std::string line;
	while (std::getline(input, line))
	{
		output << HtmlDecode(line) << "\n";
	}
}