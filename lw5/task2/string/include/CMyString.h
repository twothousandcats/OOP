#pragma once

#include <cstddef>
#include <ostream>
#include <istream>
#include <string>
#include <cassert>
#include <algorithm>
#include <compare>

#include "IteratorBase.h"

class CMyString
{
public:
	// base it template
	using Iterator = CMyStringIteratorBase<false, false>;
	using ConstIterator = CMyStringIteratorBase<true, false>;
	using ReverseIterator = CMyStringIteratorBase<false, true>;
	using ConstReverseIterator = CMyStringIteratorBase<true, true>;

	CMyString();

	CMyString(const char* pString);

	CMyString(const char* pString, size_t length);

	CMyString(const CMyString& other);

	CMyString(CMyString&& other) noexcept;

	CMyString(const std::string& stlString);

	~CMyString();

	// methods
	size_t GetLength() const noexcept;

	const char* GetStringData() const noexcept;

	CMyString SubString(size_t start, size_t length = SIZE_MAX) const;

	void Clear() noexcept;

	size_t GetCapacity() const noexcept;

	CMyString& operator=(const CMyString& other);

	CMyString& operator=(CMyString&& other) noexcept;

	// idx access
	char operator[](size_t index) const;

	char& operator[](size_t index);

	// cmp
	std::strong_ordering operator<=>(const CMyString& other) const noexcept;

	CMyString operator+(const CMyString& other) const;

	CMyString& operator+=(const CMyString& other);

	// its
	Iterator begin() noexcept;

	Iterator end() noexcept;

	ConstIterator begin() const noexcept;

	ConstIterator end() const noexcept;

	ConstIterator cbegin() const noexcept;

	ConstIterator cend() const noexcept;

	ReverseIterator rbegin() noexcept;

	ReverseIterator rend() noexcept;

	ConstReverseIterator rbegin() const noexcept;

	ConstReverseIterator rend() const noexcept;

	ConstReverseIterator crbegin() const noexcept;

	ConstReverseIterator crend() const noexcept;

	friend CMyString operator+(const char* lhs, const CMyString& rhs);

	friend CMyString operator+(const std::string& lhs, const CMyString& rhs);

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

// Non-member declarations
CMyString operator+(const char* lhs, const CMyString& rhs);

CMyString operator+(const std::string& lhs, const CMyString& rhs);