#pragma once

#include <iomanip>

namespace calc {

class Value {
public:
	Value();
	explicit Value(double val);

	static Value nan();

	bool isDefined() const;
	double get() const;

	void print(std::ostream& os) const;

	static Value add(const Value& lhs, const Value& rhs);
	static Value sub(const Value& lhs, const Value& rhs);
	static Value mul(const Value& lhs, const Value& rhs);
	static Value div(const Value& lhs, const Value& rhs);

private:
	double m_data;
};

// Operator overloading
std::ostream& operator<<(std::ostream& os, const Value& v);

} // namespace calc