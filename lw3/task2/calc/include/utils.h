#pragma once
#include <string>

namespace calc {
inline bool isValidIdentifier(const std::string& name) {
	if (name.empty()) {
		return false;
	}

	// first symbol letter or under
	if (const char first = name[0]; !std::isalpha(static_cast<unsigned char>(first)) && first != '_') {
		return false;
	}

	for (size_t i = 1; i < name.length(); ++i) {
		char c = name[i];
		if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
			return false;
		}
	}

	return true;
}

} // namespace calc