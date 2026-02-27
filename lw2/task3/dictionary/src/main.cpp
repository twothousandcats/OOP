#include "Dictionary.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <algorithm>
#include <cctype>

constexpr auto EXIT_COMMAND = "...";
constexpr auto AGREE_CHARS = "Yy";
constexpr auto SKIP_BUFFER_SIZE = 10000;

struct Args
{
	std::string dictionaryPath;
	bool showHelp = false;
};

void PrintHelp()
{
	std::cout << "Usage:\n"
		<< "  WinOS:     dictionary.exe <file> | dictionary.exe -h\n"
		<< "  MacOS/Linux: ./dictionary <file> | ./dictionary -h\n"
		<< "\nIf the file does not exist, an empty dictionary will be created.\n"
		<< "Type '" << EXIT_COMMAND << "' to exit.\n";
}

// Trim whitespace from both ends of a string
std::string Trim(const std::string& str)
{
	const auto start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
	{
		return "";
	}
	const auto end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}

// Parse comma-separated values into a vector of trimmed strings
std::vector<std::string> ParseVariants(const std::string& input)
{
	std::vector<std::string> variants;
	std::stringstream ss(input);
	std::string part;

	while (std::getline(ss, part, ','))
	{
		if (std::string clean = Trim(part); !clean.empty())
		{
			variants.push_back(clean);
		}
	}
	return variants;
}

// Print vector elements separated by ", "
void PrintList(const std::vector<std::string>& items)
{
	for (size_t i = 0; i < items.size(); ++i)
	{
		std::cout << items[i] << (i + 1 < items.size() ? ", " : "");
	}
	std::cout << "\n";
}

bool AskToSave()
{
	std::cout << "Changes were made. Save before exiting? (Y/n): ";

	char choice{};
	if (!(std::cin >> choice))
	{
		return false;
	}
	// Clear buffer including newline after char input
	std::cin.ignore(SKIP_BUFFER_SIZE, '\n');

	return std::string(1, choice).find_first_of(AGREE_CHARS) != std::string::npos;
}

bool ParseArgs(int argc, char* argv[], Args& args)
{
	if (argc != 2)
	{
		return false;
	}

	const std::string arg = argv[1];
	if (arg == "-h")
	{
		args.showHelp = true;
		return true;
	}

	args.dictionaryPath = arg;
	return true;
}

std::optional<bool> RunInteractiveSession(Dictionary& dictionary)
{
	std::cout << "Dictionary loaded. Enter words to translate (or '" << EXIT_COMMAND << "' to exit).\n";

	while (true)
	{
		std::cout << "> ";
		std::string input;
		if (!std::getline(std::cin, input))
		{
			break; // EOF
		}

		input = Trim(input);
		if (input.empty())
			continue;
		if (input == EXIT_COMMAND)
			break;

		// Try EN -> RU
		auto translations = dictionary.TranslateToRussian(input);
		if (!translations.empty())
		{
			PrintList(translations);
			continue;
		}

		// Try RU -> EN
		translations = dictionary.TranslateToEnglish(input);
		if (!translations.empty())
		{
			PrintList(translations);
			continue;
		}

		// Word not found: ask for new translation
		std::cout << "Unknown word \"" << input << "\". Enter translation(s) separated by commas (or empty to skip):\n";

		std::string newTranslationInput;
		if (!std::getline(std::cin, newTranslationInput))
		{
			break; // EOF during input
		}

		auto variants = ParseVariants(newTranslationInput);
		if (variants.empty())
		{
			std::cout << "Skipped.\n";
			continue;
		}

		for (const auto& variant : variants)
		{
			dictionary.AddTranslation(input, variant);
		}

		std::cout << "Saved \"" << input << "\" as: ";
		PrintList(variants);
	}

	if (!dictionary.IsModified())
	{
		return std::nullopt;
	}

	return AskToSave();
}

bool SaveDictionary(const Dictionary& dictionary, const std::string& path)
{
	if (dictionary.SaveToFile(path) == Dictionary::Status::Success)
	{
		std::cout << "Changes saved. Goodbye.\n";
		return true;
	}

	std::cerr << "Error: Failed to save changes to \"" << path << "\"\n";
	return false;
}

int main(int argc, char* argv[])
{
	Args args;
	if (!ParseArgs(argc, argv, args))
	{
		std::cerr << "Invalid arguments. Use '-h' for help.\n";
		return 1;
	}

	if (args.showHelp)
	{
		PrintHelp();
		return 0;
	}

	Dictionary dictionary;
	// Assuming LoadFromFile returns void or status we ignore for simplicity as per original logic
	// If it can fail critically, check status here.
	dictionary.LoadFromFile(args.dictionaryPath);

	const auto sessionResult = RunInteractiveSession(dictionary);

	if (!sessionResult.has_value())
	{
		// No changes made
		std::cout << "Goodbye.\n";
		return 0;
	}

	// check value
	if (*sessionResult)
	{
		return SaveDictionary(dictionary, args.dictionaryPath) ? 0 : 1;
	}

	std::cout << "Changes discarded. Goodbye.\n";
	return 0;
}