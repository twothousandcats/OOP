#pragma once
// todo: посмотреть концепты!
#include "CMyArrayIterator.h"

#include <algorithm>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T> class CMyArray
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using reference = T&;
	using const_reference = const T&;

	using iterator = CMyArrayIterator<T, false, false>;
	using const_iterator = CMyArrayIterator<T, true, false>;
	using reverse_iterator = CMyArrayIterator<T, false, true>;
	using const_reverse_iterator = CMyArrayIterator<T, true, true>;

	CMyArray() = default;

	CMyArray(const CMyArray& other)
		: m_data(AllocateRaw(other.m_size))
		  , m_size(0)
		  , m_capacity(other.m_size)
	{
		try
		{
			for (size_type i = 0; i < other.m_size; ++i)
			{
				ConstructAt(m_data + i, other.m_data[i]);
				++m_size;
			}
		}
		catch (...)
		{
			DestroyAndDeallocate();
			throw;
		}
	}

	// todo: T - может не иметь move
	// можно поправить концептами
	CMyArray(CMyArray&& other) noexcept
		: m_data(std::exchange(other.m_data, nullptr))
		  , m_size(std::exchange(other.m_size, 0))
		  , m_capacity(std::exchange(other.m_capacity, 0))
	{
	}

	~CMyArray() noexcept
	{
		DestroyAndDeallocate();
	}

	CMyArray& operator=(const CMyArray& other)
	{
		if (this != &other)
		{
			CMyArray tmp(other); // if copy throws, *this is unchanged
			Swap(tmp);
		}

		return *this;
	}

	CMyArray& operator=(CMyArray&& other) noexcept
	{
		if (this != &other)
		{
			DestroyAndDeallocate();
			m_data = std::exchange(other.m_data, nullptr);
			m_size = std::exchange(other.m_size, 0);
			m_capacity = std::exchange(other.m_capacity, 0);
		}

		return *this;
	}

	size_type GetSize() const noexcept { return m_size; }
	size_type GetCapacity() const noexcept { return m_capacity; }
	bool IsEmpty() const noexcept { return m_size == 0; }

	void PushBack(const T& value)
	{
		EmplaceBackImpl(value);
	}

	void PushBack(T&& value)
	{
		EmplaceBackImpl(std::move(value));
	}

	void Resize(const size_type newSize)
	{
		if (newSize < m_size)
		{
			for (size_type i = newSize; i < m_size; ++i)
			{
				m_data[i].~T();
			}
			m_size = newSize;
			return;
		}

		if (newSize > m_capacity)
		{
			Reallocate(newSize);
		}

		// Construct default values;
		// if one throws, roll back the ones we just added
		size_type constructed = m_size;
		try
		{
			for (size_type i = m_size; i < newSize; ++i)
			{
				ConstructAt(m_data + i);
				++constructed;
			}
		}
		catch (...)
		{
			for (size_type i = m_size; i < constructed; ++i)
			{
				m_data[i].~T();
			}
			throw;
		}
		m_size = newSize;
	}

	void Clear() noexcept
	{
		for (size_type i = 0; i < m_size; ++i)
		{
			m_data[i].~T();
		}
		m_size = 0;
	}

	reference operator[](size_type index)
	{
		if (index >= m_size)
		{
			throw std::out_of_range("CMyArray index out of range");
		}

		return m_data[index];
	}

	const_reference operator[](size_type index) const
	{
		if (index >= m_size)
		{
			throw std::out_of_range("CMyArray index out of range");
		}

		return m_data[index];
	}

	iterator begin() noexcept { return iterator(m_data); }
	iterator end() noexcept { return iterator(m_data + m_size); }
	const_iterator begin() const noexcept { return const_iterator(m_data); }
	const_iterator end() const noexcept { return const_iterator(m_data + m_size); }
	const_iterator cbegin() const noexcept { return begin(); }
	const_iterator cend() const noexcept { return end(); }

	reverse_iterator rbegin() noexcept { return reverse_iterator(m_data + m_size); }
	reverse_iterator rend() noexcept { return reverse_iterator(m_data); }
	const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(m_data + m_size); }
	const_reverse_iterator rend() const noexcept { return const_reverse_iterator(m_data); }
	const_reverse_iterator crbegin() const noexcept { return rbegin(); }
	const_reverse_iterator crend() const noexcept { return rend(); }

private:
	// todo: изучить концепты и применить
	static T* AllocateRaw(const size_type count)
	{
		if (count == 0)
		{
			return nullptr;
		}

		return static_cast<T*>(::operator new(count * sizeof(T)));
	}

	// todo: точно ли стоит разделять?
	// мб можно только delete
	static void DeallocateRaw(T* ptr) noexcept
	{
		::operator delete(ptr);
	}

	template <typename... Args>
	static void ConstructAt(T* location, Args&&... args)
	{
		::new(static_cast<void*>(location)) T(std::forward<Args>(args)...);
	}

	void DestroyAndDeallocate() noexcept
	{
		// todo: ~ может бросать, надо обратботать
		for (size_type i = 0; i < m_size; ++i)
		{
			m_data[i].~T();
		}
		DeallocateRaw(std::exchange(m_data, nullptr));
		m_size = 0;
		m_capacity = 0;
	}

	void Swap(CMyArray& other) noexcept
	{
		std::swap(m_data, other.m_data);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);
	}

	// Strong guarantee: builds a new buffer
	// on failure the current state is intact
	void Reallocate(const size_type newCapacity)
	{
		T* newData = AllocateRaw(newCapacity);
		size_type constructed = 0;
		try
		{
			for (size_type i = 0; i < m_size; ++i)
			{
				// Use move only if it cannot throw
				// otherwise copy to preserve strong guarantee
				ConstructAt(newData + i, std::move_if_noexcept(m_data[i]));
				++constructed;
			}
		}
		catch (...)
		{
			for (size_type i = 0; i < constructed; ++i)
			{
				newData[i].~T();
			}
			DeallocateRaw(newData);
			throw;
		}

		// Destroy old elements and free old storage
		for (size_type i = 0; i < m_size; ++i)
		{
			m_data[i].~T();
		}
		DeallocateRaw(m_data);

		m_data = newData;
		m_capacity = newCapacity;
	}

	template <typename U>
	void EmplaceBackImpl(U&& value)
	{
		if (m_size == m_capacity)
		{
			const size_type newCapacity = (m_capacity == 0)
				? 1
				: m_capacity * 2;
			Reallocate(newCapacity);
		}
		ConstructAt(m_data + m_size, std::forward<U>(value));
		++m_size;
	}

	T* m_data = nullptr;
	size_type m_size = 0;
	size_type m_capacity = 0;
};