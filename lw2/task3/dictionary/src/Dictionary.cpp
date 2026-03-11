#include "../include/Dictionary.h"
#include <fstream>
#include <algorithm>
#include <ranges>
#include <sstream>

constexpr char DELIMITER = '|';

std::string Dictionary::ToLower(std::string str)
{
	for (size_t i = 0; i < str.length();)
	{
		auto ch = static_cast<unsigned char>(str[i]);
		if (ch < 128)
		{
			str[i] = static_cast<char>(std::tolower(ch));
			++i;
		}
		else if ((ch & 0xE0) == 0xC0 && i + 1 < str.length())
		{
			const auto secondByte = static_cast<unsigned char>(str[i + 1]);
			if (ch == 0xD0)
			{
				if (secondByte >= 0x90 && secondByte <= 0xAF)
				{
					if (secondByte <= 0x9F)
					{
						str[i + 1] += 0x20;
					}
					else
					{
						str[i] = 0xD1;
						str[i + 1] -= 0x20;
					}
				}
				else if (secondByte == 0x81)
				{
					str[i] = 0xD1;
					str[i + 1] = 0x91;
				}
				else if (secondByte >= 0x80 && secondByte <= 0x8F)
				{
					str[i] = 0xD1;
					str[i + 1] += 0x10;
				}
			}
			i += 2;
		}
		else
		{
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
	str.erase(end, str.end());
	str.erase(str.begin(), begin);

	return str;
}

void Dictionary::LoadFromFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file for reading: " + filePath);
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
}

void Dictionary::SaveToFile(const std::string& filePath) const
{
	std::ofstream file(filePath);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file for writing: " + filePath);
	}

	for (const auto& [originalKey, translations] : m_forwardMap | std::views::values)
	{
		for (const auto& translation : translations)
		{
			file << originalKey << DELIMITER << translation << "\n";
		}
	}
}

std::vector<std::string> Dictionary::TranslateForward(const std::string& word) const
{
	const std::string key = ToLower(word);
	if (const auto it = m_forwardMap.find(key); it != m_forwardMap.end())
	{
		return it->second.translations;
	}
	return {};
}

std::vector<std::string> Dictionary::TranslateBackward(const std::string& word) const
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

	auto& [originalKey, translations] = m_forwardMap[normSource];
	if (originalKey.empty())
	{
		originalKey = source;
	}

	if (const auto exists = std::ranges::find(translations, target); exists == translations.end())
	{
		translations.push_back(target);
	}

	m_reverseMap[normTarget].insert(source);

	return true;
}