#include "../include/Dictionary.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <ranges>

constexpr char DELIMITER = '|';

std::string Dictionary::ToLower(std::string str)
{
	for (size_t i = 0; i < str.length();)
	{
		auto ch = static_cast<unsigned char>(str[i]); // > 127
		if (ch < 128)
		{
			// ascii
			str[i] = static_cast<char>(std::tolower(ch));
			++i;
		}
		else if ((ch & 0xE0) == 0xC0 && i + 1 < str.length())
		{
			// UTF-8 a sequence of 2 bytes starts with 110 bits
			// Processing Cyrillic (range 0xD0)
			const auto secondByte = static_cast<unsigned char>(str[i + 1]);
			if (ch == 0xD0) // 11010000
			{
				if (secondByte >= 0x90 && secondByte <= 0xAF)
				{
					// Capital letters А-Я
					if (secondByte <= 0x9F)
					{
						// А-П: add 0x20 to second
						str[i + 1] += 0x20;
					}
					else
					{
						// Р-Я: changing first to 0xD1 and sub 0x20 from second
						str[i] = 0xD1;
						str[i + 1] -= 0x20;
					}
				}
				else if (secondByte == 0x81)
				{
					// Ё -> ё
					// D081 -> D1 91
					str[i] = 0xD1;
					str[i + 1] = 0x91;
				}
				else if (secondByte >= 0x80 && secondByte <= 0x8F)
				{
					// others
					str[i] = 0xD1;
					str[i + 1] += 0x10;
				}
			}
			i += 2;
		}
		else
		{
			// Skip others symbol > two bytes
			++i;
			while (i < str.length() && (static_cast<unsigned char>(str[i]) & 0xC0) == 0x80)
			{
				++i;
			}
		}
	}
	return str;
}

std::string Dictionary::Trim(std::string str)
{
	const auto notSpace = [](const unsigned char ch) {
		return !std::isspace(ch);
	};

	const auto begin = std::ranges::find_if(str, notSpace);
	if (begin == str.end())
	{
		return "";
	}

	const auto end = std::find_if(str.rbegin(), str.rend(), notSpace).base();
	// remove range
	str.erase(end, str.end());
	str.erase(str.begin(), begin);

	return str;
}

bool Dictionary::LoadFromFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		const auto delimiterPos = line.find(DELIMITER);
		if (delimiterPos == std::string::npos)
		{
			continue;
		}

		const std::string source = Trim(line.substr(0, delimiterPos));
		const std::string target = Trim(line.substr(delimiterPos + 1));
		if (!source.empty() && !target.empty())
		{
			AddTranslation(source, target);
		}
	}

	m_isModified = false;
	return true;
}

Dictionary::Status Dictionary::SaveToFile(const std::string& filePath) const
{
	std::ofstream file(filePath);
	if (!file.is_open())
	{
		return Status::FileError;
	}

	// structured bindings
	// to iterate over the map values at once, ignoring the keys. Just values
	for (const auto& [originalKey, translations] : m_forwardMap | std::views::values)
	{
		for (const auto& translation : translations)
		{
			file << originalKey << DELIMITER << translation << "\n";
		}
	}

	return Status::Success;
}

// TranslateToRussian + TranslateToEnglish => single func?
// Self-documenting code?
std::vector<std::string> Dictionary::TranslateToRussian(const std::string& word) const
{
	const std::string key = ToLower(word);
	if (const auto it = m_forwardMap.find(key); it != m_forwardMap.end())
	{
		return it->second.translations;
	}

	return {};
}

std::vector<std::string> Dictionary::TranslateToEnglish(const std::string& word) const
{
	const std::string key = ToLower(word);
	if (const auto it = m_reverseMap.find(key); it != m_reverseMap.end())
	{
		return { it->second.begin(), it->second.end() };
	}

	return {};
}

bool Dictionary::AddTranslation(const std::string& source, const std::string& target)
{
	if (source.empty() || target.empty())
	{
		return false;
	}

	m_isModified = true;
	const std::string normSource = ToLower(source);
	const std::string normTarget = ToLower(target);

	// Update forward map
	auto& [originalKey, translations] = m_forwardMap[normSource];
	if (originalKey.empty())
	{
		originalKey = source;
	}

	if (const auto exists = std::ranges::find(translations, target); exists == translations.end())
	{
		translations.push_back(target);
	}

	// Update reverse map
	m_reverseMap[normTarget].insert(source);

	return true;
}