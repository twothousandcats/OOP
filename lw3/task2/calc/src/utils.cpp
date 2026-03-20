#include "utils.h"
#include <string>
#include <sstream>

namespace calc
{

bool isValidIdentifier(const std::string& name)
{
	if (name.empty())
	{
		return false;
	}

	if (const char first = name[0]; !std::isalpha(static_cast<unsigned char>(first)) && first != '_')
	{
		return false;
	}

	for (size_t i = 1; i < name.length(); ++i)
	{
		if (const char ch = name[i]; !std::isalnum(static_cast<unsigned char>(ch)) && ch != '_')
		{
			return false;
		}
	}

	return true;
}

std::string trim(const std::string& str)
{
	const auto start = str.find_first_not_of(" \t");
	if (start == std::string::npos)
	{
		return "";
	}
	const auto end = str.find_last_not_of(" \t");
	return str.substr(start, end - start + 1);
}

bool checkExtraChars(std::istringstream& iss)
{
	if (std::string remainder; iss >> remainder)
	{
		return true;
	}

	return false;
};

} // namespace calc