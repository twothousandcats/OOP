#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "../include/Dictionary.h"
#include <fstream>
#include <filesystem>

const std::string TEST_FILE_PATH = "test_dict_temp.txt";

void CleanupTestFile()
{
	if (std::filesystem::exists(TEST_FILE_PATH))
	{
		std::filesystem::remove(TEST_FILE_PATH);
	}
}

TEST_CASE("Dictionary loads basic entries", "[load]")
{
	CleanupTestFile();
	{
		std::ofstream out(TEST_FILE_PATH);
		out << "cat|кот\n";
		out << "dog|собака\n";
	}

	Dictionary dict;
	REQUIRE(dict.LoadFromFile(TEST_FILE_PATH));

	const auto result = dict.TranslateToRussian("cat");
	REQUIRE(result.size() == 1);
	CHECK(result[0] == "кот");

	CleanupTestFile();
}

TEST_CASE("Dictionary handles case insensitivity", "[bonus-case]")
{
	CleanupTestFile();
	Dictionary dict;
	dict.AddTranslation("cat", "кот");

	CHECK(dict.TranslateToRussian("CaT").size() == 1);
	CHECK(dict.TranslateToRussian("CaT")[0] == "кот");
	CHECK(dict.TranslateToRussian("CAT")[0] == "кот");

	CleanupTestFile();
}

TEST_CASE("Dictionary supports multiple translations", "[bonus-multi]")
{
	Dictionary dict;
	dict.AddTranslation("run", "бежать");
	dict.AddTranslation("run", "управлять");
	dict.AddTranslation("run", "бежать"); // Duplicate check

	const auto result = dict.TranslateToRussian("run");
	REQUIRE(result.size() == 2);

	CHECK(std::find(result.begin(), result.end(), "бежать") != result.end());
	CHECK(std::find(result.begin(), result.end(), "управлять") != result.end());
}

TEST_CASE("Dictionary supports bidirectional translation", "[bonus-bidir]")
{
	Dictionary dict;
	dict.AddTranslation("cat", "кот");
	dict.AddTranslation("cat", "кошка");

	const auto enToRu = dict.TranslateToRussian("cat");
	REQUIRE(enToRu.size() == 2);

	const auto ruToEnCat = dict.TranslateToEnglish("кот");
	REQUIRE(ruToEnCat.size() == 1);
	CHECK(ruToEnCat[0] == "cat");

	const auto ruToEnKoshka = dict.TranslateToEnglish("кошка");
	REQUIRE(ruToEnKoshka.size() == 1);
	CHECK(ruToEnKoshka[0] == "cat");
}

TEST_CASE("Dictionary saves and reloads correctly", "[save]")
{
	CleanupTestFile();
	{
		Dictionary dict;
		dict.AddTranslation("apple", "яблоко");
		dict.AddTranslation("Apple", "Яблоко"); // Different case key, same logic

		REQUIRE(dict.SaveToFile(TEST_FILE_PATH) == Dictionary::Status::Success);
	}

	Dictionary loadedDict;
	REQUIRE(loadedDict.LoadFromFile(TEST_FILE_PATH));

	// Should find translation regardless of input case
	CHECK(!loadedDict.TranslateToRussian("apple").empty());

	CleanupTestFile();
}

TEST_CASE("Empty input handling", "[edge]")
{
	Dictionary dict;
	CHECK(dict.TranslateToRussian("").empty());
	CHECK_FALSE(dict.AddTranslation("", "test"));
	CHECK_FALSE(dict.AddTranslation("test", ""));
}