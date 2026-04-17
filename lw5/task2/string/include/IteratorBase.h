#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

// Helper to determine pointer/reference types based on constness
template <bool IsConst> struct CMyStringIteratorTraits;

template <> struct CMyStringIteratorTraits<false>
{
	using pointer = char*;
	using reference = char&;
};

template <> struct CMyStringIteratorTraits<true>
{
	using pointer = const char*;
	using reference = const char&;
};

// Base template for iterator types
template <bool IsConst, bool IsReverse> class CMyStringIteratorBase
{
public:
	// stl
	using traits = CMyStringIteratorTraits<IsConst>;
	using iterator_category = std::random_access_iterator_tag;
	using value_type = char;
	using difference_type = std::ptrdiff_t;
	using pointer = traits::pointer;
	using reference = traits::reference;

	CMyStringIteratorBase() noexcept
		: m_ptr(nullptr)
	{
	}

	explicit CMyStringIteratorBase(pointer ptr) noexcept
		: m_ptr(ptr)
	{
	}

	// Substitution Failure Is Not An Error, it -> const it
	// current - const
	// other - !const
	// same dir
	template <bool OtherConst, bool OtherReverse, std::enable_if_t<IsConst && !OtherConst && IsReverse == OtherReverse, int> = 0> explicit CMyStringIteratorBase(const CMyStringIteratorBase<OtherConst, OtherReverse>& other) noexcept
		: m_ptr(other.GetPtr())
	{
	}

	reference operator*() const noexcept { return *m_ptr; }
	pointer operator->() const noexcept { return m_ptr; }
	reference operator[](difference_type n) const noexcept { return m_ptr[IsReverse ? -n : n]; }

	// ++it
	CMyStringIteratorBase& operator++() noexcept
	{
		if constexpr (IsReverse)
		{
			--m_ptr;
		}
		else
		{
			++m_ptr;
		}
		return *this;
	}

	CMyStringIteratorBase& operator--() noexcept
	{
		if constexpr (IsReverse)
		{
			++m_ptr;
		}
		else
		{
			--m_ptr;
		}
		return *this;
	}

	// it++
	CMyStringIteratorBase operator++(int) noexcept
	{
		auto tmp = *this;
		++(*this);
		return tmp;
	}

	CMyStringIteratorBase operator--(int) noexcept
	{
		auto tmp = *this;
		--(*this);
		return tmp;
	}

	// it += n
	CMyStringIteratorBase& operator+=(difference_type n) noexcept
	{
		if constexpr (IsReverse)
		{
			m_ptr -= n;
		}
		else
		{
			m_ptr += n;
		}
		return *this;
	}

	CMyStringIteratorBase& operator-=(difference_type n) noexcept
	{
		if constexpr (IsReverse)
		{
			m_ptr += n;
		}
		else
		{
			m_ptr -= n;
		}
		return *this;
	}

	// it += n
	CMyStringIteratorBase operator+(difference_type n) const noexcept
	{
		auto tmp = *this;
		tmp += n;
		return tmp;
	}

	CMyStringIteratorBase operator-(difference_type n) const noexcept
	{
		auto tmp = *this;
		tmp -= n;
		return tmp;
	}

	difference_type operator-(const CMyStringIteratorBase& other) const noexcept
	{
		if constexpr (IsReverse)
		{
			return other.m_ptr - m_ptr;
		}
		else
		{
			return m_ptr - other.m_ptr;
		}
	}

	bool operator==(const CMyStringIteratorBase& other) const noexcept { return m_ptr == other.m_ptr; }

	auto operator<=>(const CMyStringIteratorBase& other) const noexcept
	{
		if constexpr (IsReverse)
		{
			return other.m_ptr <=> m_ptr;
		}
		else
		{
			return m_ptr <=> other.m_ptr;
		}
	}

	// raw
	pointer GetPtr() const noexcept { return m_ptr; }

private:
	pointer m_ptr;
};

// number + iterator
template <bool IsConst, bool IsReverse> CMyStringIteratorBase<IsConst, IsReverse> operator+(
	typename CMyStringIteratorBase<IsConst, IsReverse>::difference_type n,
	const CMyStringIteratorBase<IsConst, IsReverse>& it) noexcept
{
	return it + n;
}