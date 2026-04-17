#include "CMyString.h"
#include <cstring>
#include <stdexcept>

char CMyString::m_emptyString = '\0';

CMyString::CMyString()
	: m_data(&m_emptyString)
	  , m_length(0)
	  , m_capacity(0)
{
}

// from const char* to (const char*, const size_t)
CMyString::CMyString(const char* pString)
	: CMyString(pString, pString ? std::strlen(pString) : 0)
{
}

// primary
CMyString::CMyString(const char* pString, const size_t length)
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
		// empty state
		m_data = &m_emptyString;
	}
}

// copy
CMyString::CMyString(const CMyString& other)
	: CMyString(other.m_data, other.m_length)
{
}

// move
CMyString::CMyString(CMyString&& other) noexcept
	: m_data(other.m_data)
	  , m_length(other.m_length)
	  , m_capacity(other.m_capacity)
{
	other.m_data = &m_emptyString;
	other.m_length = 0;
	other.m_capacity = 0;
}

// std::string -> primary
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

size_t CMyString::GetCapacity() const noexcept
{
	return m_capacity;
}

const char* CMyString::GetStringData() const noexcept
{
	return m_data;
}

CMyString CMyString::SubString(const size_t start, const size_t length) const
{
	if (start >= m_length)
	{
		return CMyString();
	}

	const size_t actualLength = std::min(length, m_length - start);
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

CMyString& CMyString::operator=(const CMyString& other)
{
	if (this != &other)
	{
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

char CMyString::operator[](const size_t index) const
{
	return m_data[index];
}

char& CMyString::operator[](const size_t index)
{
	return m_data[index];
}

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
	{
		return std::strong_ordering::less;
	}
	if (m_length > other.m_length)
	{
		return std::strong_ordering::greater;
	}
	return std::strong_ordering::equal;
}

bool CMyString::operator==(const CMyString& other) const noexcept
{
	if (m_length != other.m_length)
	{
		return false;
	}
	if (m_length == 0)
	{
		return true;
	}
	return std::memcmp(m_data, other.m_data, m_length) == 0;
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

	const size_t newLength = m_length + other.m_length;
	EnsureCapacity(newLength);
	std::memcpy(m_data + m_length, other.m_data, other.m_length);
	m_length = newLength;
	m_data[m_length] = '\0';
	return *this;
}

void CMyString::EnsureCapacity(const size_t newCapacity)
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

	const auto newData = new char[targetCapacity + 1];
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

void CMyString::CopyFrom(const char* source, const size_t length)
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

// free operators
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

// it methods
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