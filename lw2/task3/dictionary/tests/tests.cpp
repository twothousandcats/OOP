#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "../include/Dictionary.h"
#include <fstream>
#include <filesystem>

// исключил необязательные вызовы CleanupTestFile
const std::string TEST_FILE_PATH = "test_dict_temp.txt";

void CleanupTestFile()
{
	if (std::filesystem::exists(TEST_FILE_PATH))
	{
		std::filesystem::remove(TEST_FILE_PATH);
	}
}

TEST_CASE("Dictionary loads basic entries", "[default]")
{
	CleanupTestFile();
	{
		std::ofstream out(TEST_FILE_PATH);
		out << "cat|кот\n";
		out << "dog|собака\n";
	}

	Dictionary dict;
	REQUIRE_NOTHROW(dict.LoadFromFile(TEST_FILE_PATH));

	const auto result = dict.TranslateForward("cat");
	REQUIRE(result.size() == 1);
	CHECK(result[0] == "кот");
}

TEST_CASE("Dictionary handles case insensitivity", "[case-sensitive]")
{
	CleanupTestFile();
	Dictionary dict;
	dict.AddTranslation("cat", "кот");

	CHECK(dict.TranslateForward("CaT").size() == 1);
	CHECK(dict.TranslateForward("CaT")[0] == "кот");
	CHECK(dict.TranslateForward("CAT")[0] == "кот");
}

TEST_CASE("Dictionary supports multiple translations", "[no-rewrite]")
{
	CleanupTestFile();
	Dictionary dict;
	dict.AddTranslation("run", "бежать");
	dict.AddTranslation("run", "управлять");

	const auto result = dict.TranslateForward("run");
	REQUIRE(result.size() == 2);

	CHECK(std::find(result.begin(), result.end(), "бежать") != result.end());
	CHECK(std::find(result.begin(), result.end(), "управлять") != result.end());
}

TEST_CASE("Dictionary supports bidirectional translation", "[bidirectional]")
{
	CleanupTestFile();
	Dictionary dict;
	dict.AddTranslation("cat", "кот");
	dict.AddTranslation("cat", "кошка");

	const auto enToRu = dict.TranslateForward("cat");
	REQUIRE(enToRu.size() == 2);

	const auto ruToEnCat = dict.TranslateBackward("кот");
	REQUIRE(ruToEnCat.size() == 1);
	CHECK(ruToEnCat[0] == "cat");

	const auto ruToEnKoshka = dict.TranslateBackward("кошка");
	REQUIRE(ruToEnKoshka.size() == 1);
	CHECK(ruToEnKoshka[0] == "cat");
}

TEST_CASE("Dictionary saves and reloads correctly", "[save]")
{
	CleanupTestFile();
	{
		Dictionary dict;
		dict.AddTranslation("apple", "яблоко");
		dict.AddTranslation("Apple", "Яблоко");

		REQUIRE_NOTHROW(dict.SaveToFile(TEST_FILE_PATH));
	}

	Dictionary loadedDict;
	REQUIRE_NOTHROW(loadedDict.LoadFromFile(TEST_FILE_PATH));

	CHECK(!loadedDict.TranslateForward("apple").empty());
}

TEST_CASE("Empty input handling", "[edge]")
{
	Dictionary dict;
	CHECK(dict.TranslateForward("").empty());
	CHECK_FALSE(dict.AddTranslation("", "test"));
	CHECK_FALSE(dict.AddTranslation("test", ""));
}

TEST_CASE("LoadFromFile handles malformed lines", "[file-parsing]")
{
	CleanupTestFile();
	std::ofstream out(TEST_FILE_PATH);
	out << "valid|word\n";
	out << "no_delimiter\n";
	out << "|empty_source\n";
	out << "empty_target|\n";
	out << "  spaced | target \n";
	out << "\n";
	out.close();

	Dictionary dict;
	REQUIRE_NOTHROW(dict.LoadFromFile(TEST_FILE_PATH));

	CHECK(dict.TranslateForward("valid").size() == 1);
	CHECK(dict.TranslateForward("spaced").size() == 1);
	CHECK(dict.TranslateForward("spaced")[0] == "target");

	CHECK(dict.TranslateForward("no_delimiter").empty());
	CHECK(dict.TranslateForward("empty_source").empty());
}

TEST_CASE("LoadFromFile throws on missing file", "[file-error]")
{
	CleanupTestFile();
	Dictionary dict;
	CHECK_THROWS_AS(dict.LoadFromFile("non_existent_file_12345.txt"), std::runtime_error);
}

TEST_CASE("SaveToFile throws on invalid path", "[file-error]")
{
	CleanupTestFile();
	Dictionary dict;
	dict.AddTranslation("test", "тест");
	CHECK_THROWS_AS(dict.SaveToFile("/non_existent_dir/subdir/file.txt"), std::runtime_error);
}

TEST_CASE("Dictionary handles Cyrillic (Ё/ё) case insensitivity", "[utf8-yo]")
{
	CleanupTestFile();
	Dictionary dict;
	dict.AddTranslation("cat", "Котёнок");

	auto result = dict.TranslateForward("cat");
	REQUIRE(result.size() == 1);
	CHECK(result[0] == "Котёнок");

	CHECK(dict.TranslateBackward("котёнок").size() == 1);
	CHECK(dict.TranslateBackward("КОТЁНОК").size() == 1);
	CHECK(dict.TranslateBackward("котенок").empty());
}

TEST_CASE("IsModified flag tracks changes correctly", "[state]")
{
	CleanupTestFile();
    Dictionary dict;

    CHECK_FALSE(dict.IsModified());

    dict.AddTranslation("one", "один");
    CHECK(dict.IsModified());

    CleanupTestFile();
    std::ofstream out(TEST_FILE_PATH);
    out << "two|два\n";
    out.close();

    REQUIRE_NOTHROW(dict.LoadFromFile(TEST_FILE_PATH));
    CHECK_FALSE(dict.IsModified());

    dict.AddTranslation("three", "три");
    CHECK(dict.IsModified());

    CleanupTestFile();
}

TEST_CASE("Reverse translation handles multiple sources for one target", "[reverse-map]")
{
	CleanupTestFile();
    Dictionary dict;
    dict.AddTranslation("world", "мир");
    dict.AddTranslation("peace", "мир");

    auto result = dict.TranslateBackward("мир");

    REQUIRE(result.size() == 2);
    CHECK(std::find(result.begin(), result.end(), "world") != result.end());
    CHECK(std::find(result.begin(), result.end(), "peace") != result.end());
}