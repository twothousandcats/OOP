#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

// перешел к исключениям
// переименовал методы прямого и обратного
class Dictionary
{
public:
    Dictionary() = default;

    // Throws std::runtime_error if file cannot be opened
    void LoadFromFile(const std::string& filePath);

    // Throws std::runtime_error if file cannot be opened
    void SaveToFile(const std::string& filePath) const;

    [[nodiscard]] std::vector<std::string> TranslateForward(const std::string& word) const;

    [[nodiscard]] std::vector<std::string> TranslateBackward(const std::string& word) const;

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
    std::map<std::string, TranslationEntry> m_forwardMap;
    // Maps normalized lowercase target -> set of original source keys
    std::map<std::string, std::set<std::string>> m_reverseMap;

    bool m_isModified = false;
};