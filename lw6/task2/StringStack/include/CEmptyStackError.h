#pragma once
#include <stdexcept>

// Thrown when Top() or Pop() is called on an empty stack
class CEmptyStackError final : public std::logic_error
{
public:
	CEmptyStackError()
		: std::logic_error("Operation is not allowed on an empty stack")
	{
	}
};