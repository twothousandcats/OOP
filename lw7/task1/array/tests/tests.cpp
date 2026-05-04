#include <catch2/catch_all.hpp>

#include "CMyArray.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>

TEST_CASE("default constructed array is empty")
{
	CMyArray<int> a;
	REQUIRE(a.GetSize() == 0);
	REQUIRE(a.IsEmpty());
	REQUIRE(a.begin() == a.end());
}

TEST_CASE("PushBack appends elements and grows capacity by doubling")
{
	CMyArray<int> a;
	REQUIRE(a.GetCapacity() == 0);

	a.PushBack(1);
	REQUIRE(a.GetSize() == 1);
	REQUIRE(a.GetCapacity() == 1);

	a.PushBack(2);
	REQUIRE(a.GetCapacity() == 2);

	a.PushBack(3);
	REQUIRE(a.GetCapacity() == 4);

	a.PushBack(4);
	a.PushBack(5);
	REQUIRE(a.GetCapacity() == 8);

	REQUIRE(a[0] == 1);
	REQUIRE(a[4] == 5);
}

TEST_CASE("operator[] throws std::out_of_range")
{
	CMyArray<int> a;
	a.PushBack(10);
	REQUIRE_THROWS_AS(a[1], std::out_of_range);
	REQUIRE_THROWS_AS(a[100], std::out_of_range);

	const CMyArray<int>& ca = a;
	REQUIRE_THROWS_AS(ca[1], std::out_of_range);
}

TEST_CASE("Resize grows with default values and shrinks")
{
	CMyArray<int> a;
	a.Resize(3);
	REQUIRE(a.GetSize() == 3);
	REQUIRE(a[0] == 0);
	REQUIRE(a[1] == 0);
	REQUIRE(a[2] == 0);

	a[0] = 7;
	a[1] = 8;
	a[2] = 9;

	a.Resize(5);
	REQUIRE(a.GetSize() == 5);
	REQUIRE(a[0] == 7);
	REQUIRE(a[3] == 0);
	REQUIRE(a[4] == 0);

	a.Resize(2);
	REQUIRE(a.GetSize() == 2);
	REQUIRE(a[0] == 7);
	REQUIRE(a[1] == 8);
}

TEST_CASE("Clear removes elements but keeps capacity")
{
	CMyArray<int> a;
	for (int i = 0; i < 5; ++i)
	{
		a.PushBack(i);
	}
	const auto cap = a.GetCapacity();

	a.Clear();
	REQUIRE(a.GetSize() == 0);
	REQUIRE(a.IsEmpty());
	REQUIRE(a.GetCapacity() == cap);
}

TEST_CASE("copy constructor produces independent copy")
{
	CMyArray<std::string> a;
	a.PushBack("alpha");
	a.PushBack("beta");

	CMyArray<std::string> b(a);
	REQUIRE(b.GetSize() == 2);
	REQUIRE(b[0] == "alpha");
	REQUIRE(b[1] == "beta");

	b[0] = "changed";
	REQUIRE(a[0] == "alpha");
}

TEST_CASE("copy assignment is strong: self-assignment is safe")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);

	a = a;
	REQUIRE(a.GetSize() == 2);
	REQUIRE(a[0] == 1);
	REQUIRE(a[1] == 2);
}

TEST_CASE("copy assignment overwrites previous content")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);
	a.PushBack(3);

	CMyArray<int> b;
	b.PushBack(99);

	b = a;
	REQUIRE(b.GetSize() == 3);
	REQUIRE(b[0] == 1);
	REQUIRE(b[2] == 3);
}

TEST_CASE("move constructor leaves source empty")
{
	CMyArray<std::string> a;
	a.PushBack("x");
	a.PushBack("y");

	CMyArray<std::string> b(std::move(a));
	REQUIRE(b.GetSize() == 2);
	REQUIRE(b[1] == "y");
	REQUIRE(a.GetSize() == 0);
	REQUIRE(a.GetCapacity() == 0);
}

TEST_CASE("move assignment leaves source empty")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);

	CMyArray<int> b;
	b.PushBack(99);

	b = std::move(a);
	REQUIRE(b.GetSize() == 2);
	REQUIRE(b[0] == 1);
	REQUIRE(a.GetSize() == 0);
}

TEST_CASE("forward iteration with range-based for")
{
	CMyArray<int> a;
	for (int i = 1; i <= 5; ++i)
	{
		a.PushBack(i);
	}
	int sum = 0;
	for (int v : a)
	{
		sum += v;
	}
	REQUIRE(sum == 15);
}

TEST_CASE("reverse iteration produces reversed sequence")
{
	CMyArray<int> a;
	for (int i = 1; i <= 4; ++i)
	{
		a.PushBack(i);
	}

	std::vector<int> reversed;
	for (auto it = a.rbegin(); it != a.rend(); ++it)
	{
		reversed.push_back(*it);
	}
	REQUIRE(reversed == std::vector<int>{4, 3, 2, 1});
}

TEST_CASE("iterator supports random access")
{
	CMyArray<int> a;
	for (int i = 0; i < 10; ++i)
	{
		a.PushBack(i);
	}

	auto it = a.begin();
	REQUIRE(*(it + 3) == 3);
	REQUIRE(it[5] == 5);
	REQUIRE((a.end() - a.begin()) == 10);

	std::sort(a.begin(), a.end(), std::greater<int>());
	REQUIRE(a[0] == 9);
	REQUIRE(a[9] == 0);
}

TEST_CASE("const iteration works on const array")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);
	const CMyArray<int>& ca = a;

	int sum = std::accumulate(ca.begin(), ca.end(), 0);
	REQUIRE(sum == 3);
}

namespace
{
struct ThrowingOnCopy
{
	static int s_copyCount;
	static int s_throwAfter;

	int value = 0;

	ThrowingOnCopy() = default;

	explicit ThrowingOnCopy(int v)
		: value(v)
	{
	}

	ThrowingOnCopy(const ThrowingOnCopy& other)
		: value(other.value)
	{
		if (s_copyCount >= s_throwAfter)
		{
			throw std::runtime_error("copy failure");
		}
		++s_copyCount;
	}

	ThrowingOnCopy& operator=(const ThrowingOnCopy& other)
	{
		if (s_copyCount >= s_throwAfter)
		{
			throw std::runtime_error("copy failure");
		}
		++s_copyCount;
		value = other.value;
		return *this;
	}
};

int ThrowingOnCopy::s_copyCount = 0;
int ThrowingOnCopy::s_throwAfter = 1000000;
} // namespace

TEST_CASE("copy assignment provides strong guarantee on exception")
{
	CMyArray<ThrowingOnCopy> dst;
	dst.PushBack(ThrowingOnCopy(1));
	dst.PushBack(ThrowingOnCopy(2));

	CMyArray<ThrowingOnCopy> src;
	src.PushBack(ThrowingOnCopy(10));
	src.PushBack(ThrowingOnCopy(20));
	src.PushBack(ThrowingOnCopy(30));

	ThrowingOnCopy::s_copyCount = 0;
	ThrowingOnCopy::s_throwAfter = 1; // allow one copy then throw

	REQUIRE_THROWS_AS(dst = src, std::runtime_error);

	// dst must remain unchanged
	REQUIRE(dst.GetSize() == 2);
	REQUIRE(dst[0].value == 1);
	REQUIRE(dst[1].value == 2);

	ThrowingOnCopy::s_throwAfter = 1000000;
}