#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

template <typename T, bool IsConst, bool IsReverse> class CMyArrayIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = std::conditional_t<IsConst, const T*, T*>;
	using reference = std::conditional_t<IsConst, const T&, T&>;

	CMyArrayIterator() = default;

	explicit CMyArrayIterator(pointer ptr) noexcept
		: m_ptr(ptr)
	{
	}

	// Conversion from non-const to const iterator of the same direction
	template <bool OtherConst, typename = std::enable_if_t<IsConst && !OtherConst> > CMyArrayIterator(const CMyArrayIterator<T, OtherConst, IsReverse>& other) noexcept
		: m_ptr(other.GetRaw())
	{
	}

	pointer GetRaw() const noexcept { return m_ptr; }

	reference operator*() const noexcept
	{
		return IsReverse ? *(m_ptr - 1) : *m_ptr;
	}

	pointer operator->() const noexcept
	{
		return IsReverse ? (m_ptr - 1) : m_ptr;
	}

	reference operator[](difference_type n) const noexcept
	{
		return *(*this + n);
	}

	CMyArrayIterator& operator++() noexcept
	{
		IsReverse ? --m_ptr : ++m_ptr;
		return *this;
	}

	CMyArrayIterator operator++(int) noexcept
	{
		auto tmp = *this;
		++(*this);
		return tmp;
	}

	CMyArrayIterator& operator--() noexcept
	{
		IsReverse ? ++m_ptr : --m_ptr;
		return *this;
	}

	CMyArrayIterator operator--(int) noexcept
	{
		auto tmp = *this;
		--(*this);
		return tmp;
	}

	CMyArrayIterator& operator+=(difference_type n) noexcept
	{
		m_ptr += IsReverse ? -n : n;
		return *this;
	}

	CMyArrayIterator& operator-=(difference_type n) noexcept
	{
		return *this += -n;
	}

	friend CMyArrayIterator operator+(CMyArrayIterator it, difference_type n) noexcept
	{
		return it += n;
	}

	friend CMyArrayIterator operator+(difference_type n, CMyArrayIterator it) noexcept
	{
		return it += n;
	}

	friend CMyArrayIterator operator-(CMyArrayIterator it, difference_type n) noexcept
	{
		return it -= n;
	}

	friend difference_type operator-(const CMyArrayIterator& a, const CMyArrayIterator& b) noexcept
	{
		return IsReverse ? (b.m_ptr - a.m_ptr) : (a.m_ptr - b.m_ptr);
	}

	friend bool operator==(const CMyArrayIterator& a, const CMyArrayIterator& b) noexcept
	{
		return a.m_ptr == b.m_ptr;
	}

	friend std::strong_ordering operator<=>(const CMyArrayIterator& a, const CMyArrayIterator& b) noexcept
	{
		return IsReverse ? (b.m_ptr <=> a.m_ptr) : (a.m_ptr <=> b.m_ptr);
	}

private:
	pointer m_ptr = nullptr;
};