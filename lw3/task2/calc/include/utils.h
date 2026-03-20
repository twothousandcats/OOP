#pragma once
#include <string>
#include <sstream>

namespace calc
{

bool isValidIdentifier(const std::string& name);

std::string trim(const std::string& str);

bool checkExtraChars(std::istringstream& iss);

} // namespace calc