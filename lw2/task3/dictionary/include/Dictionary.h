#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

class Dictionary
{
public:
	enum class Status
	{
		Success,
		FileError
	};

	Dictionary() = default;

	bool LoadFromFile(const std::string& filePath);

	[[nodiscard]] Status SaveToFile(const std::string& filePath) const;

	[[nodiscard]] std::vector<std::string> TranslateToRussian(const std::string& word) const;

	[[nodiscard]] std::vector<std::string> TranslateToEnglish(const std::string& word) const;

	[[nodiscard]] bool IsModified() const { return m_isModified; }

	// Returns true if a new translation was added
	bool AddTranslation(const std::string& source, const std::string& target);

private:
	struct TranslationEntry
	{
		std::string originalKey;
		std::vector<std::string> translations;
	};

	static std::string ToLower(std::string str);

	static std::string Trim(std::string str);

	// Maps normalized lowercase source -> Entry
	// word -> list of translations
	std::map<std::string, TranslationEntry> m_forwardMap;
	// Maps normalized lowercase target -> set of original source keys
	// which word -> translations
	std::map<std::string, std::set<std::string> > m_reverseMap;
	bool m_isModified = false;
};