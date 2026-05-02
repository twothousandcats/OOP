#include "CMyArray.h"

#include <iostream>
#include <string>

namespace
{
template <typename T>
void PrintArray(const CMyArray<T>& arr, const std::string& title)
{
	std::cout << title << " (size=" << arr.GetSize()
			  << ", capacity=" << arr.GetCapacity() << "): ";
	for (const auto& item : arr)
	{
		std::cout << item << ' ';
	}
	std::cout << '\n';
}
} // namespace

int main()
{
	CMyArray<std::string> strings;
	strings.PushBack("hello");
	strings.PushBack("world");
	strings.PushBack("from");
	strings.PushBack("CMyArray");
	PrintArray(strings, "strings");

	CMyArray<std::string> stringsCopy = strings;
	stringsCopy[0] = "HELLO";
	PrintArray(stringsCopy, "stringsCopy");
	PrintArray(strings, "strings (unchanged)");

	std::cout << "strings reversed: ";
	for (auto it = strings.rbegin(); it != strings.rend(); ++it)
	{
		std::cout << *it << ' ';
	}
	std::cout << '\n';

	CMyArray<double> numbers;
	for (int i = 1; i <= 5; ++i)
	{
		numbers.PushBack(i * 1.5);
	}
	PrintArray(numbers, "numbers");

	numbers.Resize(8);
	PrintArray(numbers, "numbers after Resize(8)");

	numbers.Resize(3);
	PrintArray(numbers, "numbers after Resize(3)");

	numbers.Clear();
	PrintArray(numbers, "numbers after Clear");

	return 0;
}