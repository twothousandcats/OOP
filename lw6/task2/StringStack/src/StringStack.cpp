#include "StringStack.h"

#include <utility>

struct CStringStack::Node
{
	std::string value;
	Node* next = nullptr;
};

CStringStack::CStringStack(const CStringStack& other)
{
	CopyFrom(other);
}

CStringStack::CStringStack(CStringStack&& other) noexcept
	: m_top(other.m_top)
	  , m_size(other.m_size)
{
	other.m_top = nullptr;
	other.m_size = 0;
}

CStringStack& CStringStack::operator=(const CStringStack& other)
{
	if (this != &other)
	{
		CStringStack tmp(other); // if this throws, *this is intact
		Swap(tmp); // noexcept
	}
	return *this;
}

CStringStack& CStringStack::operator=(CStringStack&& other) noexcept
{
	if (this != &other)
	{
		Clear();
		m_top = other.m_top;
		m_size = other.m_size;
		other.m_top = nullptr;
		other.m_size = 0;
	}
	return *this;
}

CStringStack::~CStringStack() noexcept
{
	Clear();
}

// on failure the stack is unchanged
void CStringStack::Push(const std::string& value)
{
	// if this throws, state is intact
	auto* newNode = new Node{ value, m_top };

	// noexcept
	m_top = newNode;
	++m_size;
}

void CStringStack::Push(std::string&& value)
{
	auto* newNode = new Node{ std::move(value), m_top };
	m_top = newNode;
	++m_size;
}

const std::string& CStringStack::Top() const
{
	EnsureNotEmpty();
	return m_top->value;
}

void CStringStack::Pop()
{
	EnsureNotEmpty();
	const Node* oldTop = m_top;
	m_top = m_top->next;
	--m_size;
	delete oldTop; // std::string destructor is noexcept
}

void CStringStack::Clear() noexcept
{
	// Iterative traversal avoids deep recursion on long stacks.
	while (m_top != nullptr)
	{
		Node* next = m_top->next;
		delete m_top;
		m_top = next;
	}
	m_size = 0;
}

bool CStringStack::IsEmpty() const noexcept
{
	return m_top == nullptr;
}

size_t CStringStack::GetSize() const noexcept
{
	return m_size;
}

void CStringStack::EnsureNotEmpty() const
{
	if (m_top == nullptr)
	{
		throw CEmptyStackError();
	}
}

void CStringStack::Swap(CStringStack& other) noexcept
{
	std::swap(m_top, other.m_top);
	std::swap(m_size, other.m_size);
}

// Builds an independent deep copy of `other` into *this
void CStringStack::CopyFrom(const CStringStack& other)
{
	if (other.m_top == nullptr)
	{
		return;
	}
	try
	{
		m_top = new Node{ other.m_top->value, nullptr };
		Node* tail = m_top;
		for (const Node* src = other.m_top->next; src != nullptr; src = src->next)
		{
			tail->next = new Node{ src->value, nullptr };
			tail = tail->next;
		}
		m_size = other.m_size;
	}
	catch (...)
	{
		Clear(); // release whatever was already allocated
		throw;
	}
}