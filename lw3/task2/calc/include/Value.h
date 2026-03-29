#pragma once

#include <ostream>

namespace calc
{

class Value
{
public:
	Value();

	explicit Value(double val);

	static Value Nan();

	bool IsNan() const;

	double Get() const;

	void Print(std::ostream& os) const;

	// исключить статик -> no invariant
	static Value Add(const Value& lhs, const Value& rhs);

	static Value Sub(const Value& lhs, const Value& rhs);

	static Value Mul(const Value& lhs, const Value& rhs);

	static Value Div(const Value& lhs, const Value& rhs);

private:
	double m_data;
};

} // namespace calc