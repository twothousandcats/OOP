#include <catch2/catch_all.hpp>
#include "StringStack.h"

#include <string>
#include <utility>

TEST_CASE("newly created stack is empty")
{
	const CStringStack stack;
	CHECK(stack.IsEmpty());
	CHECK(stack.GetSize() == 0);
}

TEST_CASE("Top on empty stack throws CEmptyStackError")
{
	const CStringStack stack;
	CHECK_THROWS_AS(stack.Top(), CEmptyStackError);
}

TEST_CASE("Pop on empty stack throws CEmptyStackError")
{
	CStringStack stack;
	CHECK_THROWS_AS(stack.Pop(), CEmptyStackError);
}

TEST_CASE("Push increments size and updates Top")
{
	CStringStack stack;
	stack.Push("hello");
	CHECK_FALSE(stack.IsEmpty());
	CHECK(stack.GetSize() == 1);
	CHECK(stack.Top() == "hello");

	stack.Push("world");
	CHECK(stack.GetSize() == 2);
	CHECK(stack.Top() == "world");
}

TEST_CASE("stack preserves LIFO order")
{
	CStringStack stack;
	stack.Push("a");
	stack.Push("b");
	stack.Push("c");

	CHECK(stack.Top() == "c");
	stack.Pop();
	CHECK(stack.Top() == "b");
	stack.Pop();
	CHECK(stack.Top() == "a");
	stack.Pop();
	CHECK(stack.IsEmpty());
	CHECK(stack.GetSize() == 0);
}

// todo: разобраться почему правильно отрабатывает
TEST_CASE("rvalue Push works correctly")
{
	CStringStack stack;
	std::string source = "movable";
	stack.Push(std::move(source));
	CHECK(stack.Top() == "movable");
	CHECK(stack.GetSize() == 1);
}

TEST_CASE("Clear empties the stack")
{
	CStringStack stack;
	stack.Push("a");
	stack.Push("b");
	stack.Clear();
	CHECK(stack.IsEmpty());
	CHECK(stack.GetSize() == 0);
	CHECK_THROWS_AS(stack.Top(), CEmptyStackError);
}

TEST_CASE("stack can be reused after Clear")
{
	CStringStack stack;
	stack.Push("a");
	stack.Clear();
	stack.Push("b");
	CHECK(stack.Top() == "b");
	CHECK(stack.GetSize() == 1);
}

TEST_CASE("copy constructor produces an independent deep copy")
{
	CStringStack original;
	original.Push("one");
	original.Push("two");
	original.Push("three");

	CStringStack copy(original);

	CHECK(copy.GetSize() == 3);
	CHECK(copy.Top() == "three");

	// mutates
	copy.Pop();
	CHECK(copy.Top() == "two");
	CHECK(original.GetSize() == 3);
	CHECK(original.Top() == "three");
}

TEST_CASE("copy of an empty stack is also empty")
{
	CStringStack empty;
	CStringStack copy(empty);
	CHECK(copy.IsEmpty());
	CHECK(copy.GetSize() == 0);
}

TEST_CASE("copy assignment replaces contents and leaves original intact")
{
	CStringStack a;
	a.Push("x");
	a.Push("y");

	CStringStack b;
	b.Push("garbage");
	b = a;

	CHECK(b.GetSize() == 2);
	CHECK(b.Top() == "y");
	b.Pop();
	CHECK(b.Top() == "x");

	// unchanged
	CHECK(a.GetSize() == 2);
	CHECK(a.Top() == "y");
}

TEST_CASE("self copy-assignment is safe")
{
	CStringStack stack;
	stack.Push("a");
	stack.Push("b");

	const CStringStack& alias = stack;
	stack = alias;

	CHECK(stack.GetSize() == 2);
	CHECK(stack.Top() == "b");
}

TEST_CASE("move constructor transfers ownership and leaves source empty")
{
	CStringStack source;
	source.Push("a");
	source.Push("b");
	const CStringStack target(std::move(source));

	CHECK(target.GetSize() == 2);
	CHECK(target.Top() == "b");
	CHECK(source.IsEmpty());
	CHECK(source.GetSize() == 0);
}

TEST_CASE("move assignment transfers ownership and leaves source empty")
{
	CStringStack source;
	source.Push("a");
	source.Push("b");

	CStringStack target;
	target.Push("to_be_released");
	target = std::move(source);

	CHECK(target.GetSize() == 2);
	CHECK(target.Top() == "b");
	CHECK(source.IsEmpty());
}

TEST_CASE("self move-assignment is safe")
{
	CStringStack stack;
	stack.Push("a");

	CStringStack& alias = stack;
	stack = std::move(alias);

	// stack is in a valid state
	CHECK_NOTHROW(stack.IsEmpty());
}

TEST_CASE("move operations are declared noexcept")
{
	STATIC_REQUIRE(std::is_nothrow_move_constructible_v<CStringStack>);
	STATIC_REQUIRE(std::is_nothrow_move_assignable_v<CStringStack>);
	STATIC_REQUIRE(std::is_nothrow_destructible_v<CStringStack>);
}

TEST_CASE("handles a large number of elements without deep recursion")
{
	CStringStack stack;
	constexpr size_t count = 100000;
	for (size_t i = 0; i < count; ++i)
	{
		stack.Push(std::to_string(i));
	}
	CHECK(stack.GetSize() == count);
	CHECK(stack.Top() == std::to_string(count - 1));
	// destruct
}

TEST_CASE("Pop failure on empty stack does not change state")
{
	CStringStack stack;
	try
	{
		stack.Pop();
	}
	catch (const CEmptyStackError&)
	{
	}
	CHECK(stack.IsEmpty());
	CHECK(stack.GetSize() == 0);
}