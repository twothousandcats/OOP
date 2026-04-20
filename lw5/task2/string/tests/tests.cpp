#include <catch2/catch_all.hpp>
#include "CMyString.h"
#include <sstream>
#include <cstring>
#include <algorithm>
// todo: разименование итераторов

TEST_CASE("Default constructor creates empty string", "[CMyString]")
{
	const CMyString str;
	REQUIRE(str.GetLength() == 0);
	REQUIRE(str.GetCapacity() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE("Constructor from null-terminated string", "[CMyString]")
{
	const CMyString str("Hello");
	REQUIRE(str.GetLength() == 5);
	REQUIRE(strcmp(str.GetStringData(), "Hello") == 0);
}

TEST_CASE("Constructor from null-terminated string with empty string", "[CMyString]")
{
	CMyString str("");
	REQUIRE(str.GetLength() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE("Constructor from nullptr", "[CMyString]")
{
	CMyString str(nullptr);
	REQUIRE(str.GetLength() == 0);
	REQUIRE(str.GetCapacity() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE("Constructor from nullptr with zero length", "[CMyString]")
{
	CMyString str(nullptr, 0);
	REQUIRE(str.GetLength() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE("Constructor from nullptr with non-zero length produces empty string", "[CMyString]")
{
	// nullptr have to be 0 regardless of length
	CMyString str(nullptr, 5);
	REQUIRE(str.GetLength() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE("Constructor from char array with length", "[CMyString]")
{
	CMyString str("Hello\0World", 11);
	REQUIRE(str.GetLength() == 11);
	REQUIRE(memcmp(str.GetStringData(), "Hello\0World", 11) == 0);
	REQUIRE(str.GetStringData()[11] == '\0');
}

TEST_CASE("Constructor from char array with zero length", "[CMyString]")
{
	CMyString str("Hello", 0);
	REQUIRE(str.GetLength() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE(
	"Constructor from std::string"
	,
	"[CMyString]"
	)
{
	std::string stlStr = "Test String";
	CMyString str(stlStr);
	REQUIRE(str.GetLength() == 11);
	REQUIRE(strcmp(str.GetStringData(), "Test String") == 0);
}

TEST_CASE(
	"Constructor from empty std::string"
	,
	"[CMyString]"
	)
{
	std::string stlStr;
	CMyString str(stlStr);
	REQUIRE(str.GetLength() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE("Copy constructor", "[CMyString]")
{
	CMyString original("Original");
	CMyString copy(original);
	REQUIRE(copy.GetLength() == original.GetLength());
	REQUIRE(strcmp(copy.GetStringData(), original.GetStringData()) == 0);

	// verify
	REQUIRE(copy.GetStringData() != original.GetStringData());
}

TEST_CASE(
	"Copy constructor from empty string"
	,
	"[CMyString]"
	)
{
	CMyString original;
	CMyString copy(original);
	REQUIRE(copy.GetLength() == 0);
	REQUIRE(strcmp(copy.GetStringData(), "") == 0);
}

TEST_CASE(
	"Move constructor"
	,
	"[CMyString]"
	)
{
	CMyString original("Movable");
	CMyString moved(std::move(original));

	REQUIRE(moved.GetLength() == 7);
	REQUIRE(strcmp(moved.GetStringData(), "Movable") == 0);

	REQUIRE(original.GetLength() == 0);
	REQUIRE(strcmp(original.GetStringData(), "") == 0);
}

TEST_CASE("Destructor cleans up memory", "[CMyString]")
{
	{
		CMyString str("Temporary");
		REQUIRE(str.GetLength() == 9);
	}

	// have to detect memory leak?
}

TEST_CASE(
	"GetLength returns correct length"
	,
	"[CMyString]"
	)
{
	CMyString str("12345");
	REQUIRE(str.GetLength() == 5);
}

TEST_CASE(
	"GetCapacity grows when needed"
	,
	"[CMyString]"
	)
{
	CMyString str("1234567890"); // 10 chars
	size_t initialCapacity = str.GetCapacity();
	REQUIRE(initialCapacity >= 10);

	str += CMyString("1"); // Now 11 chars, capacity should double
	REQUIRE(str.GetCapacity() >= 20);
}

TEST_CASE(
	"GetStringData returns null-terminated string"
	,
	"[CMyString]"
	)
{
	CMyString str("Test");
	const char* data = str.GetStringData();
	REQUIRE(data[4] == '\0');
}

TEST_CASE(
	"GetStringData works with embedded nulls"
	,
	"[CMyString]"
	)
{
	CMyString str("A\0B", 3);
	const char* data = str.GetStringData();
	REQUIRE(data[0] == 'A');
	REQUIRE(data[1] == '\0');
	REQUIRE(data[2] == 'B');
	REQUIRE(data[3] == '\0'); // term
}

TEST_CASE(
	"SubString extracts substring correctly"
	,
	"[CMyString]"
	)
{
	CMyString str("Hello World");
	CMyString sub = str.SubString(6, 5);
	REQUIRE(sub.GetLength() == 5);
	REQUIRE(strcmp(sub.GetStringData(), "World") == 0);
}

TEST_CASE(
	"SubString with default length extracts to end"
	,
	"[CMyString]"
	)
{
	CMyString str("Hello World");
	CMyString sub = str.SubString(6);
	REQUIRE(sub.GetLength() == 5);
	REQUIRE(strcmp(sub.GetStringData(), "World") == 0);
}

TEST_CASE(
	"SubString with start beyond length returns empty"
	,
	"[CMyString]"
	)
{
	CMyString str("Hello");
	CMyString sub = str.SubString(10);
	REQUIRE(sub.GetLength() == 0);
}

TEST_CASE(
	"SubString with length exceeding available chars"
	,
	"[CMyString]"
	)
{
	CMyString str("Hello");
	CMyString sub = str.SubString(2, 100);
	REQUIRE(sub.GetLength() == 3);
	REQUIRE(strcmp(sub.GetStringData(), "llo") == 0);
}

TEST_CASE(
	"Clear empties the string"
	,
	"[CMyString]"
	)
{
	CMyString str("To be cleared");
	str.Clear();
	REQUIRE(str.GetLength() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
	REQUIRE(str.GetCapacity() == 0);
}

TEST_CASE(
	"Clear on already-empty string"
	,
	"[CMyString]"
	)
{
	CMyString str;
	str.Clear();
	REQUIRE(str.GetLength() == 0);
	REQUIRE(str.GetCapacity() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE(
	"Copy assignment operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("First");
	CMyString str2("Second");
	str2 = str1;

	REQUIRE(str2.GetLength() == 5);
	REQUIRE(strcmp(str2.GetStringData(), "First") == 0);
}

TEST_CASE(
	"Copy assignment from empty to non-empty"
	,
	"[CMyString]"
	)
{
	CMyString empty;
	CMyString str("SomeValue");
	str = empty;
	REQUIRE(str.GetLength() == 0);
	REQUIRE(strcmp(str.GetStringData(), "") == 0);
}

TEST_CASE(
	"Copy assignment from non-empty to empty"
	,
	"[CMyString]"
	)
{
	CMyString empty;
	CMyString str("SomeValue");
	empty = str;
	REQUIRE(empty.GetLength() == 9);
	REQUIRE(strcmp(empty.GetStringData(), "SomeValue") == 0);
}

TEST_CASE(
	"Chained copy assignment"
	,
	"[CMyString]"
	)
{
	CMyString a("aaa");
	CMyString b("bbb");
	CMyString c("ccc");
	a = b = c;
	REQUIRE(strcmp(a.GetStringData(), "ccc") == 0);
	REQUIRE(strcmp(b.GetStringData(), "ccc") == 0);
	REQUIRE(strcmp(c.GetStringData(), "ccc") == 0);
}

TEST_CASE(
	"Self assignment"
	,
	"[CMyString]"
	)
{
	CMyString str("SelfAssign");
	str = str;
	REQUIRE(str.GetLength() == 10);
	REQUIRE(strcmp(str.GetStringData(), "SelfAssign") == 0);
}

TEST_CASE(
	"Move assignment operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("Source");
	CMyString str2("Dest");
	str2 = std::move(str1);

	REQUIRE(str2.GetLength() == 6);
	REQUIRE(strcmp(str2.GetStringData(), "Source") == 0);
	REQUIRE(str1.GetLength() == 0);
}

TEST_CASE(
	"Self move-assignment"
	,
	"[CMyString]"
	)
{
	CMyString str("SelfMove");
	str = std::move(str);
	// Must not crash or corrupt the object
	REQUIRE(str.GetLength() == 8);
	REQUIRE(strcmp(str.GetStringData(), "SelfMove") == 0);
}

TEST_CASE(
	"Const index operator"
	,
	"[CMyString]"
	)
{
	const CMyString str("Index");
	REQUIRE(str[0] == 'I');
	REQUIRE(str[4] == 'x');
}

TEST_CASE(
	"Non-const index operator"
	,
	"[CMyString]"
	)
{
	CMyString str("Index");
	str[0] = 'i';
	REQUIRE(str[0] == 'i');
}

TEST_CASE(
	"Equality operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("Equal");
	CMyString str2("Equal");
	CMyString str3("Different");

	REQUIRE(str1 == str2);
	REQUIRE(!(str1 == str3));
}

TEST_CASE(
	"Equality with empty strings"
	,
	"[CMyString]"
	)
{
	CMyString empty1;
	CMyString empty2;
	CMyString emptyLiteral("");
	CMyString nonEmpty("x");

	REQUIRE(empty1 == empty2);
	REQUIRE(empty1 == emptyLiteral);
	REQUIRE(!(empty1 == nonEmpty));
}

TEST_CASE(
	"Equality with embedded nulls distinguishes strings after null byte"
	,
	"[CMyString]"
	)
{
	// strcmp-based implementation would incorrectly say these are equal
	CMyString a("A\0B", 3);
	CMyString b("A\0C", 3);
	CMyString c("A\0B", 3);

	REQUIRE(a == c);
	REQUIRE(!(a == b));
	REQUIRE(a != b);
}

TEST_CASE(
	"Inequality operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("Equal");
	CMyString str2("Different");

	REQUIRE(str1 != str2);
	REQUIRE(!(str1 != str1));
}

TEST_CASE(
	"Less than operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("Apple");
	CMyString str2("Banana");
	CMyString str3("Apple");

	REQUIRE(str1 < str2);
	REQUIRE(!(str2 < str1));
	REQUIRE(!(str1 < str3));
}

TEST_CASE(
	"Greater than operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("Banana");
	CMyString str2("Apple");

	REQUIRE(str1 > str2);
	REQUIRE(!(str2 > str1));
}

TEST_CASE(
	"Less or equal operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("Apple");
	CMyString str2("Banana");
	CMyString str3("Apple");

	REQUIRE(str1 <= str2);
	REQUIRE(str1 <= str3);
	REQUIRE(!(str2 <= str1));
}

TEST_CASE(
	"Greater or equal operator"
	,
	"[CMyString]"
	)
{
	CMyString str1("Banana");
	CMyString str2("Apple");
	CMyString str3("Banana");

	REQUIRE(str1 >= str2);
	REQUIRE(str1 >= str3);
	REQUIRE(!(str2 >= str1));
}

TEST_CASE(
	"Comparison with empty strings"
	,
	"[CMyString]"
	)
{
	CMyString empty;
	CMyString nonEmpty("x");

	REQUIRE(empty < nonEmpty);
	REQUIRE(nonEmpty > empty);
	REQUIRE(empty <= nonEmpty);
	REQUIRE(nonEmpty >= empty);
	REQUIRE(empty == CMyString());
}

TEST_CASE(
	"Comparison with embedded nulls uses memcmp semantics"
	,
	"[CMyString]"
	)
{
	// memcmp treats \0 as a regular byte; strcmp would stop at it
	CMyString a("A\0B", 3);
	CMyString b("A\0C", 3);

	REQUIRE(a < b);
	REQUIRE(b > a);
}

TEST_CASE(
	"Spaceship operator returns strong_ordering"
	,
	"[CMyString]"
	)
{
	CMyString a("Apple");
	CMyString b("Banana");

	auto result = a <=> b;
	// Verify it's actually strong_ordering (compile-time check via type)
	static_assert(std::is_same_v<decltype(result), std::strong_ordering>);
	REQUIRE(result == std::strong_ordering::less);
	REQUIRE((a <=> a) == std::strong_ordering::equal);
}

TEST_CASE(
	"Concatenation with +"
	,
	"[CMyString]"
	)
{
	CMyString str1("Hello ");
	CMyString str2("World");
	CMyString result = str1 + str2;

	REQUIRE(result.GetLength() == 11);
	REQUIRE(strcmp(result.GetStringData(), "Hello World") == 0);
}

TEST_CASE(
	"Concatenation with +="
	,
	"[CMyString]"
	)
{
	CMyString str1("Hello ");
	CMyString str2("World");
	str1 += str2;

	REQUIRE(str1.GetLength() == 11);
	REQUIRE(strcmp(str1.GetStringData(), "Hello World") == 0);
}

TEST_CASE(
	"Concatenation with empty operand does not change result"
	,
	"[CMyString]"
	)
{
	CMyString str("NonEmpty");
	CMyString empty;

	CMyString r1 = str + empty;
	CMyString r2 = empty + str;
	REQUIRE(strcmp(r1.GetStringData(), "NonEmpty") == 0);
	REQUIRE(strcmp(r2.GetStringData(), "NonEmpty") == 0);

	str += empty;
	REQUIRE(strcmp(str.GetStringData(), "NonEmpty") == 0);
	REQUIRE(str.GetLength() == 8);
}

TEST_CASE(
	"Self-concatenation with += doubles the string"
	,
	"[CMyString]"
	)
{
	// Dangerous case: self-reference during reallocation
	CMyString str("AB");
	str += str;
	REQUIRE(str.GetLength() == 4);
	REQUIRE(strcmp(str.GetStringData(), "ABAB") == 0);
}

TEST_CASE(
	"Self-concatenation triggers reallocation safely"
	,
	"[CMyString]"
	)
{
	// Force reallocation: start small, double into new buffer
	CMyString str("Hello");
	const size_t oldCap = str.GetCapacity();
	str += str; // likely requires growth
	REQUIRE(str.GetLength() == 10);
	REQUIRE(strcmp(str.GetStringData(), "HelloHello") == 0);
	REQUIRE(str.GetCapacity() >= 10);
	REQUIRE(str.GetCapacity() > oldCap);
}

TEST_CASE(
	"Concatenation preserves data across capacity growth"
	,
	"[CMyString]"
	)
{
	CMyString str("ABCDEFGHIJ"); // 10 chars
	str += CMyString("KLMNOPQRST"); // triggers growth
	REQUIRE(str.GetLength() == 20);
	REQUIRE(strcmp(str.GetStringData(), "ABCDEFGHIJKLMNOPQRST") == 0);
}

TEST_CASE(
	"Concatenation with const char*"
	,
	"[CMyString]"
	)
{
	CMyString str("World");
	CMyString result = "Hello " + str;

	REQUIRE(result.GetLength() == 11);
	REQUIRE(strcmp(result.GetStringData(), "Hello World") == 0);
}

TEST_CASE(
	"Concatenation with std::string"
	,
	"[CMyString]"
	)
{
	CMyString str("World");
	std::string prefix = "Hello ";
	CMyString result = prefix + str;

	REQUIRE(result.GetLength() == 11);
	REQUIRE(strcmp(result.GetStringData(), "Hello World") == 0);
}

TEST_CASE(
	"Output stream operator"
	,
	"[CMyString]"
	)
{
	CMyString str("Stream");
	std::ostringstream oss;
	oss << str;
	REQUIRE(oss.str() == "Stream");
}

TEST_CASE(
	"Output stream writes embedded nulls as bytes"
	,
	"[CMyString]"
	)
{
	CMyString str("A\0B", 3);
	std::ostringstream oss;
	oss << str;
	std::string result = oss.str();
	REQUIRE(result.size() == 3);
	REQUIRE(result[0] == 'A');
	REQUIRE(result[1] == '\0');
	REQUIRE(result[2] == 'B');
}

TEST_CASE(
	"Input stream operator"
	,
	"[CMyString]"
	)
{
	CMyString str;
	std::istringstream iss("Input");
	iss >> str;
	REQUIRE(str.GetLength() == 5);
	REQUIRE(strcmp(str.GetStringData(), "Input") == 0);
}

// iterators
TEST_CASE("Forward iterator begin/end", "[CMyString][Iterator]")
{
	CMyString str("Iterate");
	auto it = str.begin();
	REQUIRE(*it == 'I');
}

TEST_CASE(
	"Empty string begin equals end"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString empty;
	REQUIRE(empty.begin() == empty.end());
	REQUIRE(empty.cbegin() == empty.cend());
	REQUIRE(empty.rbegin() == empty.rend());
	REQUIRE(empty.crbegin() == empty.crend());
}

TEST_CASE(
	"Default-constructed iterator"
	,
	"[CMyString][Iterator]"
	)
{
	// random-access iterators must be default
	CMyString::Iterator it1;
	CMyString::Iterator it2;
	REQUIRE(it1 == it2);
}

TEST_CASE(
	"Forward iterator increment"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("ABC");
	auto it = str.begin();
	++it;
	REQUIRE(*it == 'B');
	++it;
	REQUIRE(*it == 'C');
}

TEST_CASE(
	"Forward iterator decrement"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("ABC");
	auto it = str.end();
	--it;
	REQUIRE(*it == 'C');
	--it;
	REQUIRE(*it == 'B');
}

TEST_CASE(
	"Iterator difference"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Distance");
	auto it1 = str.begin();
	auto it2 = str.begin() + 4;
	REQUIRE(it2 - it1 == 4);
}

TEST_CASE(
	"Iterator addition"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Addition");
	auto it = str.begin();
	it = it + 3;
	REQUIRE(*it == 'i');
}

TEST_CASE(
	"Iterator subtraction"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Subtract");
	auto it = str.begin() + 5; // to 'a'
	it = it - 3; // to 'b'
	REQUIRE(*it == 'b');
}

TEST_CASE(
	"Iterator indexing"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Indexed");
	auto it = str.begin();
	REQUIRE(it[0] == 'I');
	REQUIRE(it[3] == 'e');
	// todo: -1 выходы за
}

TEST_CASE(
	"Iterator comparison operators"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Compare");
	auto it1 = str.begin();
	auto it2 = str.begin() + 3;

	REQUIRE(it1 != it2);
	REQUIRE(it1 < it2);
	REQUIRE(it2 > it1);
	REQUIRE(it1 <= it2);
	REQUIRE(it2 >= it1);
}

TEST_CASE(
	"Range-based for loop"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Range");
	std::string result;
	for (const char ch : str)
	{
		result += ch;
	}
	REQUIRE(result == "Range");
}

TEST_CASE(
	"Const range-based for loop"
	,
	"[CMyString][Iterator]"
	)
{
	const CMyString str("ConstRange");
	std::string result;
	for (char ch : str)
	{
		result += ch;
	}
	REQUIRE(result == "ConstRange");
}

TEST_CASE(
	"cbegin"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("CBEGIN");
	auto it1 = str.cbegin();
	REQUIRE(*it1 == 'C');
}

TEST_CASE(
	"Reverse iterator begin/end"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Reverse");
	auto it = str.rbegin();
	REQUIRE(*it == 'e');
}

TEST_CASE(
	"Reverse iterator increment"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("ABC");
	auto it = str.rbegin();
	++it;
	REQUIRE(*it == 'B');
	++it;
	REQUIRE(*it == 'A');
}

TEST_CASE(
	"Reverse iterator decrement"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("ABC");
	auto it = str.rend();
	--it;
	REQUIRE(*it == 'A');
	--it;
	REQUIRE(*it == 'B');
}

TEST_CASE(
	"Const reverse iterator"
	,
	"[CMyString][Iterator]"
	)
{
	const CMyString str("ConstRev");
	auto it = str.rbegin();
	REQUIRE(*it == 'v');
}

TEST_CASE(
	"Reverse iterator difference"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("RevDist");
	auto it1 = str.rbegin();
	auto it2 = str.rbegin() + 3;
	REQUIRE(it2 - it1 == 3);
}

TEST_CASE(
	"Reverse iterator addition"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("RevAdd");
	auto it = str.rbegin();
	it = it + 2;
	REQUIRE(*it == 'A');
}

TEST_CASE(
	"Reverse iterator indexing"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("RevIdx");
	auto it = str.rbegin();
	REQUIRE(it[0] == 'x');
	REQUIRE(it[2] == 'I');
}

TEST_CASE(
	"crbegin/crend"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("CRBEGIN");
	auto it = str.crbegin();
	REQUIRE(*it == 'N');
}

TEST_CASE(
	"Reverse iteration with algorithms"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("Hello");
	std::string result;
	for (auto it = str.rbegin(); it != str.rend(); ++it)
	{
		result += *it;
	}
	REQUIRE(result == "olleH");
}

TEST_CASE(
	"std::find with iterator"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("SearchMe");
	auto it = std::find(str.begin(), str.end(), 'r');
	REQUIRE(it != str.end());
	REQUIRE(*it == 'r');
}

TEST_CASE(
	"std::count with iterator"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("CountAAAs");
	int count = std::count(str.begin(), str.end(), 'A');
	REQUIRE(count == 3);
}

TEST_CASE(
	"std::transform with iterator"
	,
	"[CMyString][Iterator]"
	)
{
	CMyString str("lower");
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	REQUIRE(strcmp(str.GetStringData(), "LOWER") == 0);
}

TEST_CASE(
	"Empty string operations"
	,
	"[CMyString]"
	)
{
	CMyString str;
	CMyString other("NotEmpty");

	REQUIRE(str + other == other);
	REQUIRE(other + str == other);
	REQUIRE(str == str);
}

TEST_CASE(
	"Large string capacity growth"
	,
	"[CMyString]"
	)
{
	CMyString str;
	for (int i = 0; i < 100; ++i)
	{
		str += CMyString("X");
	}
	REQUIRE(str.GetLength() == 100);
	REQUIRE(str.GetCapacity() >= 100);
}

TEST_CASE(
	"String with multiple embedded nulls"
	,
	"[CMyString]"
	)
{
	char buffer[] = { 'A', '\0', 'B', '\0', 'C' };
	CMyString str(buffer, 5);
	REQUIRE(str.GetLength() == 5);
	REQUIRE(str.GetStringData()[0] == 'A');
	REQUIRE(str.GetStringData()[1] == '\0');
	REQUIRE(str.GetStringData()[2] == 'B');
	REQUIRE(str.GetStringData()[3] == '\0');
	REQUIRE(str.GetStringData()[4] == 'C');
}

TEST_CASE(
	"Move then use source"
	,
	"[CMyString]"
	)
{
	CMyString str1("MoveMe");
	CMyString str2(std::move(str1));

	// Source should still be usable
	REQUIRE(str1.GetLength() == 0);
	str1 = CMyString("Reassigned");
	REQUIRE(str1.GetLength() == 10);
}

TEST_CASE(
	"Chained concatenation"
	,
	"[CMyString]"
	)
{
	CMyString result = CMyString("A") + CMyString("B") + CMyString("C");
	REQUIRE(result.GetLength() == 3);
	REQUIRE(strcmp(result.GetStringData(), "ABC") == 0);
}

TEST_CASE(
	"Comparison with different lengths"
	,
	"[CMyString]"
	)
{
	CMyString shortStr("Short");
	CMyString longStr("Shorter");

	REQUIRE(shortStr < longStr);
	REQUIRE(longStr > shortStr);
}

TEST_CASE("Substring at boundaries", "[CMyString]")
{
	CMyString str("Boundary");

	// Start at 0
	CMyString sub1 = str.SubString(0, 4);
	REQUIRE(strcmp(sub1.GetStringData(), "Boun") == 0);

	// Start at last char
	CMyString sub2 = str.SubString(7, 1);
	REQUIRE(strcmp(sub2.GetStringData(), "y") == 0);
}