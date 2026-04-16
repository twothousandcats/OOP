#pragma once

#include <cstddef>
#include <ostream>
#include <istream>
#include <string>
#include <cassert>
#include <algorithm>

class CMyString
{
public:
	// Constructor by default
	CMyString();

	// Constructor from null-terminated string
	explicit CMyString(const char* pString);

	// Constructor from char array with specified length
	CMyString(const char* pString, size_t length);

	// Copy constructor
	CMyString(const CMyString& other);

	// Move constructor
	CMyString(CMyString&& other) noexcept;

	// Constructor from std::string
	CMyString(const std::string& stlString);

	// Destructor
	~CMyString();

	// Returns string length (without null terminator)
	size_t GetLength() const;

	// Returns pointer to char array with null terminator
	[[nodiscard]] const char* GetStringData() const;

	// Returns substring from start position with max length
	CMyString SubString(size_t start, size_t length = SIZE_MAX) const;

	// Clears the string
	void Clear();

	// Returns capacity
	size_t GetCapacity() const;

	// Assignment operator
	CMyString& operator=(const CMyString& other);

	// Move assignment operator
	CMyString& operator=(CMyString&& other) noexcept;

	// Index operators (const and non-const)
	char operator[](size_t index) const;

	char& operator[](size_t index);

	// Comparison operators
	bool operator==(const CMyString& other) const;

	bool operator!=(const CMyString& other) const;

	bool operator<(const CMyString& other) const;

	bool operator>(const CMyString& other) const;

	bool operator<=(const CMyString& other) const;

	bool operator>=(const CMyString& other) const;

	// Concatenation operators
	CMyString operator+(const CMyString& other) const;

	CMyString& operator+=(const CMyString& other);

	// Iterator support
	class Iterator;
	class ConstIterator;
	class ReverseIterator;
	class ConstReverseIterator;

	Iterator begin();

	Iterator end();

	ConstIterator begin() const;

	ConstIterator end() const;

	ConstIterator cbegin() const;

	ConstIterator cend() const;

	ReverseIterator rbegin();

	ReverseIterator rend();

	ConstReverseIterator rbegin() const;

	ConstReverseIterator rend() const;

	ConstReverseIterator crbegin() const;

	ConstReverseIterator crend() const;

	// Non-member concatenation operators (for const char* and std::string on left side)
	friend CMyString operator+(const char* lhs, const CMyString& rhs);

	friend CMyString operator+(const std::string& lhs, const CMyString& rhs);

	// Stream operators
	friend std::ostream& operator<<(std::ostream& os, const CMyString& str);

	friend std::istream& operator>>(std::istream& is, CMyString& str);

private:
	char* m_data;
	size_t m_length;
	size_t m_capacity;

	static char m_emptyString;

	void EnsureCapacity(size_t newCapacity);

	void CopyFrom(const char* source, size_t length);
};

// Non-member concatenation operators (for const char* and std::string on left side)
CMyString operator+(const char* lhs, const CMyString& rhs);

CMyString operator+(const std::string& lhs, const CMyString& rhs);

// Iterator class implementation
class CMyString::Iterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = char;
	using difference_type = std::ptrdiff_t;
	using pointer = char*;
	using reference = char&;

	Iterator()
		: m_ptr(nullptr)
	{
	}

	explicit Iterator(char* ptr)
		: m_ptr(ptr)
	{
	}

	reference operator*() const { return *m_ptr; }
	pointer operator->() const { return m_ptr; }

	Iterator& operator++()
	{
		++m_ptr;
		return *this;
	}

	Iterator operator++(int)
	{
		Iterator tmp = *this;
		++(*this);
		return tmp;
	}

	Iterator& operator--()
	{
		--m_ptr;
		return *this;
	}

	Iterator operator--(int)
	{
		Iterator tmp = *this;
		--(*this);
		return tmp;
	}

	Iterator& operator+=(difference_type n)
	{
		m_ptr += n;
		return *this;
	}

	Iterator& operator-=(difference_type n)
	{
		m_ptr -= n;
		return *this;
	}

	Iterator operator+(difference_type n) const
	{
		return Iterator(m_ptr + n);
	}

	Iterator operator-(difference_type n) const
	{
		return Iterator(m_ptr - n);
	}

	difference_type operator-(const Iterator& other) const
	{
		return m_ptr - other.m_ptr;
	}

	reference operator[](difference_type n) const
	{
		return m_ptr[n];
	}

	bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
	bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
	bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }
	bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }
	bool operator<=(const Iterator& other) const { return m_ptr <= other.m_ptr; }
	bool operator>=(const Iterator& other) const { return m_ptr >= other.m_ptr; }

private:
	char* m_ptr;
};

// ConstIterator class implementation
class CMyString::ConstIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = char;
	using difference_type = std::ptrdiff_t;
	using pointer = const char*;
	using reference = const char&;

	ConstIterator()
		: m_ptr(nullptr)
	{
	}

	explicit ConstIterator(const char* ptr)
		: m_ptr(ptr)
	{
	}

	ConstIterator(const Iterator& it)
		: m_ptr(&*it)
	{
	}

	reference operator*() const { return *m_ptr; }
	pointer operator->() const { return m_ptr; }

	ConstIterator& operator++()
	{
		++m_ptr;
		return *this;
	}

	ConstIterator operator++(int)
	{
		ConstIterator tmp = *this;
		++(*this);
		return tmp;
	}

	ConstIterator& operator--()
	{
		--m_ptr;
		return *this;
	}

	ConstIterator operator--(int)
	{
		ConstIterator tmp = *this;
		--(*this);
		return tmp;
	}

	ConstIterator& operator+=(difference_type n)
	{
		m_ptr += n;
		return *this;
	}

	ConstIterator& operator-=(difference_type n)
	{
		m_ptr -= n;
		return *this;
	}

	ConstIterator operator+(difference_type n) const
	{
		return ConstIterator(m_ptr + n);
	}

	ConstIterator operator-(difference_type n) const
	{
		return ConstIterator(m_ptr - n);
	}

	difference_type operator-(const ConstIterator& other) const
	{
		return m_ptr - other.m_ptr;
	}

	reference operator[](difference_type n) const
	{
		return m_ptr[n];
	}

	bool operator==(const ConstIterator& other) const { return m_ptr == other.m_ptr; }
	bool operator!=(const ConstIterator& other) const { return m_ptr != other.m_ptr; }
	bool operator<(const ConstIterator& other) const { return m_ptr < other.m_ptr; }
	bool operator>(const ConstIterator& other) const { return m_ptr > other.m_ptr; }
	bool operator<=(const ConstIterator& other) const { return m_ptr <= other.m_ptr; }
	bool operator>=(const ConstIterator& other) const { return m_ptr >= other.m_ptr; }

private:
	const char* m_ptr;
};

// ReverseIterator class implementation
class CMyString::ReverseIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = char;
	using difference_type = std::ptrdiff_t;
	using pointer = char*;
	using reference = char&;

	ReverseIterator()
		: m_ptr(nullptr)
	{
	}

	explicit ReverseIterator(char* ptr)
		: m_ptr(ptr)
	{
	}

	reference operator*() const { return *m_ptr; }
	pointer operator->() const { return m_ptr; }

	ReverseIterator& operator++()
	{
		--m_ptr;
		return *this;
	}

	ReverseIterator operator++(int)
	{
		ReverseIterator tmp = *this;
		++(*this);
		return tmp;
	}

	ReverseIterator& operator--()
	{
		++m_ptr;
		return *this;
	}

	ReverseIterator operator--(int)
	{
		ReverseIterator tmp = *this;
		--(*this);
		return tmp;
	}

	ReverseIterator& operator+=(difference_type n)
	{
		m_ptr -= n;
		return *this;
	}

	ReverseIterator& operator-=(difference_type n)
	{
		m_ptr += n;
		return *this;
	}

	ReverseIterator operator+(difference_type n) const
	{
		return ReverseIterator(m_ptr - n);
	}

	ReverseIterator operator-(difference_type n) const
	{
		return ReverseIterator(m_ptr + n);
	}

	difference_type operator-(const ReverseIterator& other) const
	{
		return other.m_ptr - m_ptr;
	}

	reference operator[](difference_type n) const
	{
		return m_ptr[-n];
	}

	bool operator==(const ReverseIterator& other) const { return m_ptr == other.m_ptr; }
	bool operator!=(const ReverseIterator& other) const { return m_ptr != other.m_ptr; }
	bool operator<(const ReverseIterator& other) const { return m_ptr > other.m_ptr; }
	bool operator>(const ReverseIterator& other) const { return m_ptr < other.m_ptr; }
	bool operator<=(const ReverseIterator& other) const { return m_ptr >= other.m_ptr; }
	bool operator>=(const ReverseIterator& other) const { return m_ptr <= other.m_ptr; }

private:
	char* m_ptr;
};

// ConstReverseIterator class implementation
class CMyString::ConstReverseIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = char;
	using difference_type = std::ptrdiff_t;
	using pointer = const char*;
	using reference = const char&;

	ConstReverseIterator()
		: m_ptr(nullptr)
	{
	}

	explicit ConstReverseIterator(const char* ptr)
		: m_ptr(ptr)
	{
	}

	ConstReverseIterator(const ReverseIterator& it)
		: m_ptr(&*it)
	{
	}

	reference operator*() const { return *m_ptr; }
	pointer operator->() const { return m_ptr; }

	ConstReverseIterator& operator++()
	{
		--m_ptr;
		return *this;
	}

	ConstReverseIterator operator++(int)
	{
		ConstReverseIterator tmp = *this;
		++(*this);
		return tmp;
	}

	ConstReverseIterator& operator--()
	{
		++m_ptr;
		return *this;
	}

	ConstReverseIterator operator--(int)
	{
		ConstReverseIterator tmp = *this;
		--(*this);
		return tmp;
	}

	ConstReverseIterator& operator+=(difference_type n)
	{
		m_ptr -= n;
		return *this;
	}

	ConstReverseIterator& operator-=(difference_type n)
	{
		m_ptr += n;
		return *this;
	}

	ConstReverseIterator operator+(difference_type n) const
	{
		return ConstReverseIterator(m_ptr - n);
	}

	ConstReverseIterator operator-(difference_type n) const
	{
		return ConstReverseIterator(m_ptr + n);
	}

	difference_type operator-(const ConstReverseIterator& other) const
	{
		return other.m_ptr - m_ptr;
	}

	reference operator[](difference_type n) const
	{
		return m_ptr[-n];
	}

	bool operator==(const ConstReverseIterator& other) const { return m_ptr == other.m_ptr; }
	bool operator!=(const ConstReverseIterator& other) const { return m_ptr != other.m_ptr; }
	bool operator<(const ConstReverseIterator& other) const { return m_ptr > other.m_ptr; }
	bool operator>(const ConstReverseIterator& other) const { return m_ptr < other.m_ptr; }
	bool operator<=(const ConstReverseIterator& other) const { return m_ptr >= other.m_ptr; }
	bool operator>=(const ConstReverseIterator& other) const { return m_ptr <= other.m_ptr; }

private:
	const char* m_ptr;
};

// Non-member operator+ for Iterator
inline CMyString::Iterator operator+(typename CMyString::Iterator::difference_type n, const CMyString::Iterator& it)
{
	return it + n;
}

// Non-member operator+ for ConstIterator
inline CMyString::ConstIterator operator+(typename CMyString::ConstIterator::difference_type n, const CMyString::ConstIterator& it)
{
	return it + n;
}

// Non-member operator+ for ReverseIterator
inline CMyString::ReverseIterator operator+(typename CMyString::ReverseIterator::difference_type n, const CMyString::ReverseIterator& it)
{
	return it + n;
}

// Non-member operator+ for ConstReverseIterator
inline CMyString::ConstReverseIterator operator+(typename CMyString::ConstReverseIterator::difference_type n, const CMyString::ConstReverseIterator& it)
{
	return it + n;
}