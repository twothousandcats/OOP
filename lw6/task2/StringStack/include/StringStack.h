#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include "CEmptyStackError.h"

// Push, copy ctor, copy assignment:     strong
// Pop, Top:                             strong
// Move ctor, move assignment, swap:     noexcept
// Destructor, Clear, IsEmpty, GetSize:  noexcept
class CStringStack
{
public:
	CStringStack() noexcept = default;

	CStringStack(const CStringStack& other);

	CStringStack(CStringStack&& other) noexcept;

	CStringStack& operator=(const CStringStack& other);

	CStringStack& operator=(CStringStack&& other) noexcept;

	~CStringStack() noexcept;

	void Push(const std::string& value);

	void Push(std::string&& value);

	const std::string& Top() const;

	void Pop();

	void Clear() noexcept;

	bool IsEmpty() const noexcept;

	size_t GetSize() const noexcept;

private:
	struct Node;

	void EnsureNotEmpty() const;

	void Swap(CStringStack& other) noexcept;

	void CopyFrom(const CStringStack& other);

	Node* m_top = nullptr;
	size_t m_size = 0;
};