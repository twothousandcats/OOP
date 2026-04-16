#include "CMyString.h"
#include <cstring>
#include <stdexcept>

// Static empty string for moved-from objects
char CMyString::m_emptyString = '\0';

CMyString::CMyString()
	: m_data(&m_emptyString)
	  , m_length(0)
	  , m_capacity(0)
{
}

CMyString::CMyString(const char* pString)
	: m_data(nullptr)
	  , m_length(0)
	  , m_capacity(0)
{
	if (pString != nullptr)
	{
		size_t len = std::strlen(pString);
		CopyFrom(pString, len);
	}
	else
	{
		m_data = &m_emptyString;
		m_length = 0;
		m_capacity = 0;
	}
}

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
		m_data = &m_emptyString;
		m_length = 0;
		m_capacity = 0;
	}
}

CMyString::CMyString(const CMyString& other)
	: m_data(nullptr)
	  , m_length(0)
	  , m_capacity(0)
{
	CopyFrom(other.m_data, other.m_length);
}

CMyString::CMyString(CMyString&& other) noexcept
	: m_data(other.m_data)
	  , m_length(other.m_length)
	  , m_capacity(other.m_capacity)
{
	other.m_data = &m_emptyString;
	other.m_length = 0;
	other.m_capacity = 0;
}

CMyString::CMyString(const std::string& stlString)
	: m_data(nullptr)
	  , m_length(0)
	  , m_capacity(0)
{
	CopyFrom(stlString.c_str(), stlString.length());
}

CMyString::~CMyString()
{
	if (m_data != &m_emptyString)
	{
		delete[] m_data;
	}
}

size_t CMyString::GetLength() const
{
	return m_length;
}

const char* CMyString::GetStringData() const
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

void CMyString::Clear()
{
	if (m_data != &m_emptyString)
	{
		delete[] m_data;
	}
	m_data = &m_emptyString;
	m_length = 0;
	m_capacity = 0;
}

size_t CMyString::GetCapacity() const
{
	return m_capacity;
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

bool CMyString::operator==(const CMyString& other) const
{
	if (m_length != other.m_length)
	{
		return false;
	}
	return std::memcmp(m_data, other.m_data, m_length) == 0;
}

bool CMyString::operator!=(const CMyString& other) const
{
	return !(*this == other);
}

bool CMyString::operator<(const CMyString& other) const
{
	size_t minLength = std::min(m_length, other.m_length);
	int cmp = std::memcmp(m_data, other.m_data, minLength);
	if (cmp != 0)
	{
		return cmp < 0;
	}
	return m_length < other.m_length;
}

bool CMyString::operator>(const CMyString& other) const
{
	return other < *this;
}

bool CMyString::operator<=(const CMyString& other) const
{
	return !(other < *this);
}

bool CMyString::operator>=(const CMyString& other) const
{
	return !(*this < other);
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
		if (targetCapacity == 0)
		{
			targetCapacity = 1;
		}
		else
		{
			targetCapacity *= 2;
		}
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

// Non-member operator+ implementations (for const char* and std::string on left side)
CMyString operator+(const char* lhs, const CMyString& rhs)
{
	return CMyString(lhs) += rhs;
}

CMyString operator+(const std::string& lhs, const CMyString& rhs)
{
	return CMyString(lhs) += rhs;
}

// Stream operators
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

// Iterator access methods
CMyString::Iterator CMyString::begin()
{
	return Iterator(m_data);
}

CMyString::Iterator CMyString::end()
{
	return Iterator(m_data + m_length);
}

CMyString::ConstIterator CMyString::begin() const
{
	return ConstIterator(m_data);
}

CMyString::ConstIterator CMyString::end() const
{
	return ConstIterator(m_data + m_length);
}

CMyString::ConstIterator CMyString::cbegin() const
{
	return ConstIterator(m_data);
}

CMyString::ConstIterator CMyString::cend() const
{
	return ConstIterator(m_data + m_length);
}

CMyString::ReverseIterator CMyString::rbegin()
{
	return ReverseIterator(m_data + m_length - 1);
}

CMyString::ReverseIterator CMyString::rend()
{
	return ReverseIterator(m_data - 1);
}

CMyString::ConstReverseIterator CMyString::rbegin() const
{
	return ConstReverseIterator(m_data + m_length - 1);
}

CMyString::ConstReverseIterator CMyString::rend() const
{
	return ConstReverseIterator(m_data - 1);
}

CMyString::ConstReverseIterator CMyString::crbegin() const
{
	return ConstReverseIterator(m_data + m_length - 1);
}

CMyString::ConstReverseIterator CMyString::crend() const
{
	return ConstReverseIterator(m_data - 1);
}