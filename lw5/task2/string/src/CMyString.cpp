#include "CMyString.h"
#include <cstring>
#include <stdexcept>

char CMyString::m_emptyString = '\0';

// Default constructor delegates to the empty state logic
CMyString::CMyString()
	: m_data(&m_emptyString)
	  , m_length(0)
	  , m_capacity(0)
{
}

// Delegate from const char* to (const char*, size_t)
CMyString::CMyString(const char* pString)
	: CMyString(pString, pString ? std::strlen(pString) : 0)
{
}

// Primary constructor for raw data
CMyString::CMyString(const char* pString, size_t length)
	: m_data(nullptr)
	  , m_length(0)
	  , m_capacity(0)
{
	if (pString != nullptr)
	{
		CopyFrom(pString, length);
	}
	else
	{
		// Fallback to empty state if null provided with length
		m_data = &m_emptyString;
	}
}

// Copy constructor
CMyString::CMyString(const CMyString& other)
	: CMyString(other.m_data, other.m_length)
{
}

// Move constructor
CMyString::CMyString(CMyString&& other) noexcept
	: m_data(other.m_data)
	  , m_length(other.m_length)
	  , m_capacity(other.m_capacity)
{
	other.m_data = &m_emptyString;
	other.m_length = 0;
	other.m_capacity = 0;
}

// Constructor from std::string
CMyString::CMyString(const std::string& stlString)
	: CMyString(stlString.c_str(), stlString.length())
{
}

CMyString::~CMyString()
{
	if (m_data != &m_emptyString)
	{
		delete[] m_data;
	}
}

size_t CMyString::GetLength() const noexcept
{
	return m_length;
}

const char* CMyString::GetStringData() const noexcept
{
	return m_data;
}

CMyString CMyString::SubString(size_t start, size_t length) const
{
	if (start >= m_length)
	{
		return CMyString();
	}

	size_t actualLength = std::min(length, m_length - start);
	return CMyString(m_data + start, actualLength);
}

void CMyString::Clear() noexcept
{
	if (m_data != &m_emptyString)
	{
		delete[] m_data;
	}
	m_data = &m_emptyString;
	m_length = 0;
	m_capacity = 0;
}

size_t CMyString::GetCapacity() const noexcept
{
	return m_capacity;
}

CMyString& CMyString::operator=(const CMyString& other)
{
	if (this != &other)
	{
		// Copy-and-swap idiom could be used, but direct implementation is efficient here
		if (m_data != &m_emptyString)
		{
			delete[] m_data;
		}
		CopyFrom(other.m_data, other.m_length);
	}
	return *this;
}

CMyString& CMyString::operator=(CMyString&& other) noexcept
{
	if (this != &other)
	{
		if (m_data != &m_emptyString)
		{
			delete[] m_data;
		}
		m_data = other.m_data;
		m_length = other.m_length;
		m_capacity = other.m_capacity;

		other.m_data = &m_emptyString;
		other.m_length = 0;
		other.m_capacity = 0;
	}
	return *this;
}

char CMyString::operator[](size_t index) const
{
	assert(index < m_length);
	return m_data[index];
}

char& CMyString::operator[](size_t index)
{
	assert(index < m_length);
	return m_data[index];
}

// C++20 Spaceship Operator
std::strong_ordering CMyString::operator<=>(const CMyString& other) const noexcept
{
	const size_t minLength = std::min(m_length, other.m_length);
	if (minLength > 0)
	{
		int cmp = std::memcmp(m_data, other.m_data, minLength);
		if (cmp != 0)
		{
			return cmp < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
		}
	}

	if (m_length < other.m_length)
		return std::strong_ordering::less;
	if (m_length > other.m_length)
		return std::strong_ordering::greater;
	return std::strong_ordering::equal;
}

CMyString CMyString::operator+(const CMyString& other) const
{
	CMyString result(*this);
	result += other;
	return result;
}

CMyString& CMyString::operator+=(const CMyString& other)
{
	if (other.m_length == 0)
	{
		return *this;
	}

	size_t newLength = m_length + other.m_length;
	EnsureCapacity(newLength);
	std::memcpy(m_data + m_length, other.m_data, other.m_length);
	m_length = newLength;
	m_data[m_length] = '\0';
	return *this;
}

void CMyString::EnsureCapacity(size_t newCapacity)
{
	if (newCapacity <= m_capacity)
	{
		return;
	}

	size_t targetCapacity = m_capacity;
	while (targetCapacity < newCapacity)
	{
		targetCapacity = (targetCapacity == 0) ? 1 : targetCapacity * 2;
	}

	char* newData = new char[targetCapacity + 1];
	if (m_data != &m_emptyString && m_length > 0)
	{
		std::memcpy(newData, m_data, m_length);
	}
	newData[m_length] = '\0';

	if (m_data != &m_emptyString)
	{
		delete[] m_data;
	}

	m_data = newData;
	m_capacity = targetCapacity;
}

void CMyString::CopyFrom(const char* source, size_t length)
{
	if (length == 0)
	{
		m_data = &m_emptyString;
		m_length = 0;
		m_capacity = 0;
		return;
	}

	m_data = new char[length + 1];
	std::memcpy(m_data, source, length);
	m_data[length] = '\0';
	m_length = length;
	m_capacity = length;
}

// Non-member operators
CMyString operator+(const char* lhs, const CMyString& rhs)
{
	return CMyString(lhs) += rhs;
}

CMyString operator+(const std::string& lhs, const CMyString& rhs)
{
	return CMyString(lhs) += rhs;
}

std::ostream& operator<<(std::ostream& os, const CMyString& str)
{
	os.write(str.m_data, static_cast<std::streamsize>(str.m_length));
	return os;
}

std::istream& operator>>(std::istream& is, CMyString& str)
{
	std::string temp;
	is >> temp;
	str = CMyString(temp);
	return is;
}

// Iterator methods implementation
CMyString::Iterator CMyString::begin() noexcept
{
	return Iterator(m_data);
}

CMyString::Iterator CMyString::end() noexcept
{
	return Iterator(m_data + m_length);
}

CMyString::ConstIterator CMyString::begin() const noexcept
{
	return ConstIterator(m_data);
}

CMyString::ConstIterator CMyString::end() const noexcept
{
	return ConstIterator(m_data + m_length);
}

CMyString::ConstIterator CMyString::cbegin() const noexcept
{
	return ConstIterator(m_data);
}

CMyString::ConstIterator CMyString::cend() const noexcept
{
	return ConstIterator(m_data + m_length);
}

CMyString::ReverseIterator CMyString::rbegin() noexcept
{
	return ReverseIterator(m_data + m_length - 1);
}

CMyString::ReverseIterator CMyString::rend() noexcept
{
	return ReverseIterator(m_data - 1);
}

CMyString::ConstReverseIterator CMyString::rbegin() const noexcept
{
	return ConstReverseIterator(m_data + m_length - 1);
}

CMyString::ConstReverseIterator CMyString::rend() const noexcept
{
	return ConstReverseIterator(m_data - 1);
}

CMyString::ConstReverseIterator CMyString::crbegin() const noexcept
{
	return ConstReverseIterator(m_data + m_length - 1);
}

CMyString::ConstReverseIterator CMyString::crend() const noexcept
{
	return ConstReverseIterator(m_data - 1);
}