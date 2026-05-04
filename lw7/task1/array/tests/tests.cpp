#include <catch2/catch_all.hpp>

#include "CMyArray.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// todo: написать тесты для своих классов
// todo: класс должен кидать искючение в деструкторе

// Construction

TEST_CASE("default constructed array is empty")
{
	CMyArray<int> a;
	REQUIRE(a.GetSize() == 0);
	REQUIRE(a.GetCapacity() == 0);
	REQUIRE(a.IsEmpty());
	REQUIRE(a.begin() == a.end());
	REQUIRE(a.rbegin() == a.rend());
}

// PushBack / capacity growth

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

TEST_CASE("PushBack accepts rvalue")
{
	CMyArray<std::string> a;
	std::string s = "hello";
	a.PushBack(std::move(s));
	REQUIRE(a.GetSize() == 1);
	REQUIRE(a[0] == "hello");

	a.PushBack(std::string("world"));
	REQUIRE(a[1] == "world");
}

TEST_CASE("PushBack reuses capacity after Clear")
{
	CMyArray<int> a;
	for (int i = 0; i < 8; ++i)
	{
		a.PushBack(i);
	}
	const auto cap = a.GetCapacity();

	a.Clear();
	a.PushBack(42);
	REQUIRE(a.GetSize() == 1);
	REQUIRE(a.GetCapacity() == cap);
	REQUIRE(a[0] == 42);
}

// operator[]

TEST_CASE("operator[] returns element by index")
{
	CMyArray<int> a;
	a.PushBack(10);
	a.PushBack(20);

	REQUIRE(a[0] == 10);
	REQUIRE(a[1] == 20);

	a[0] = 99;
	REQUIRE(a[0] == 99);

	const CMyArray<int>& ca = a;
	REQUIRE(ca[1] == 20);
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

// Resize

TEST_CASE("Resize grows with default values and shrinks")
{
	// todo: добавить сложные классы
	// надо чтобы бросал исключения
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

TEST_CASE("Resize to zero clears elements")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);
	const auto cap = a.GetCapacity();

	a.Resize(0);
	REQUIRE(a.GetSize() == 0);
	REQUIRE(a.IsEmpty());
	REQUIRE(a.GetCapacity() == cap);
}

TEST_CASE("Resize to current size is a no-op")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);

	a.Resize(2);
	REQUIRE(a.GetSize() == 2);
	REQUIRE(a[0] == 1);
	REQUIRE(a[1] == 2);
}

TEST_CASE("Resize does not shrink capacity")
{
	CMyArray<int> a;
	for (int i = 0; i < 10; ++i)
	{
		a.PushBack(i);
	}
	const auto cap = a.GetCapacity();

	a.Resize(2);
	REQUIRE(a.GetCapacity() == cap);
}

// Clear

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

TEST_CASE("Clear on empty array is safe")
{
	CMyArray<int> a;
	a.Clear();
	REQUIRE(a.GetSize() == 0);
	REQUIRE(a.GetCapacity() == 0);
}

// Copy

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

TEST_CASE("copy constructor from empty array")
{
	CMyArray<int> a;
	CMyArray<int> b(a);
	REQUIRE(b.GetSize() == 0);
	REQUIRE(b.IsEmpty());
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

// Move

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

TEST_CASE("move constructor from empty array")
{
	CMyArray<int> a;
	CMyArray<int> b(std::move(a));
	REQUIRE(b.GetSize() == 0);
	REQUIRE(a.GetSize() == 0);
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

TEST_CASE("move self-assignment is safe")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);

	a = std::move(a);
	REQUIRE(a.GetSize() == 2);
	REQUIRE(a[0] == 1);
	REQUIRE(a[1] == 2);
}

TEST_CASE("array works with move-only element type")
{
	CMyArray<std::unique_ptr<int> > a;
	a.PushBack(std::make_unique<int>(1));
	a.PushBack(std::make_unique<int>(2));
	a.PushBack(std::make_unique<int>(3));
	a.PushBack(std::make_unique<int>(4));

	REQUIRE(a.GetSize() == 4);
	REQUIRE(*a[0] == 1);
	REQUIRE(*a[3] == 4);

	CMyArray<std::unique_ptr<int> > b(std::move(a));
	REQUIRE(b.GetSize() == 4);
	REQUIRE(*b[2] == 3);
}

// Iteration

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

TEST_CASE("const iteration works on const array")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);
	const CMyArray<int>& ca = a;

	int sum = std::accumulate(ca.begin(), ca.end(), 0);
	REQUIRE(sum == 3);
}

TEST_CASE("cbegin/cend and crbegin/crend return const iterators")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);
	a.PushBack(3);

	int sum = std::accumulate(a.cbegin(), a.cend(), 0);
	REQUIRE(sum == 6);

	std::vector<int> rev;
	for (auto it = a.crbegin(); it != a.crend(); ++it)
	{
		rev.push_back(*it);
	}
	REQUIRE(rev == std::vector<int>{3, 2, 1});
}

TEST_CASE("iterator converts from non-const to const")
{
	CMyArray<int> a;
	a.PushBack(42);

	CMyArray<int>::iterator it = a.begin();
	CMyArray<int>::const_iterator cit = it;
	REQUIRE(*cit == 42);
	REQUIRE(cit == a.cbegin());
}

TEST_CASE("iterator postfix increment and decrement")
{
	CMyArray<int> a;
	a.PushBack(1);
	a.PushBack(2);
	a.PushBack(3);

	auto it = a.begin();
	auto old = it++;
	REQUIRE(*old == 1);
	REQUIRE(*it == 2);

	auto old2 = it--;
	REQUIRE(*old2 == 2);
	REQUIRE(*it == 1);
}

TEST_CASE("iterator arithmetic and subscript")
{
	CMyArray<int> a;
	for (int i = 0; i < 10; ++i)
	{
		a.PushBack(i);
	}

	auto it = a.begin();
	REQUIRE(*(it + 3) == 3);
	REQUIRE(*(3 + it) == 3);
	REQUIRE(it[5] == 5);

	auto it2 = a.end();
	it2 -= 1;
	REQUIRE(*it2 == 9);
	REQUIRE(*(it2 - 4) == 5);

	REQUIRE((a.end() - a.begin()) == 10);
}

TEST_CASE("iterator comparison operators")
{
	CMyArray<int> a;
	for (int i = 0; i < 5; ++i)
	{
		a.PushBack(i);
	}

	auto b = a.begin();
	auto e = a.end();
	REQUIRE(b != e);
	REQUIRE(b < e);
	REQUIRE(b <= e);
	REQUIRE(e > b);
	REQUIRE(e >= b);
	REQUIRE(b <= b);
	REQUIRE(b >= b);
}

TEST_CASE("iterator operator-> works")
{
	CMyArray<std::string> a;
	a.PushBack("hello");
	a.PushBack("world");

	auto it = a.begin();
	REQUIRE(it->size() == 5);
	++it;
	REQUIRE(it->size() == 5);
}

TEST_CASE("reverse iterator supports random access")
{
	CMyArray<int> a;
	for (int i = 0; i < 5; ++i)
	{
		a.PushBack(i); // 0 1 2 3 4
	}

	auto rit = a.rbegin();
	REQUIRE(*rit == 4);
	REQUIRE(rit[1] == 3);
	REQUIRE(*(rit + 2) == 2);

	REQUIRE((a.rend() - a.rbegin()) == 5);
}

TEST_CASE("iterator works with std::sort")
{
	CMyArray<int> a;
	for (int i = 0; i < 10; ++i)
	{
		a.PushBack(i);
	}

	std::sort(a.begin(), a.end(), std::greater<int>());
	REQUIRE(a[0] == 9);
	REQUIRE(a[9] == 0);
}