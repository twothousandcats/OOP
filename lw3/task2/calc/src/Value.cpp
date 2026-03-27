#include "Value.h"
#include <iomanip>

namespace calc
{

Value::Value()
	: m_data(std::numeric_limits<double>::quiet_NaN())
{
}

Value::Value(double val)
	: m_data(val)
{
}

Value Value::Nan()
{
	return Value(std::numeric_limits<double>::quiet_NaN());
}

bool Value::IsDefined() const
{
	return !std::isnan(m_data);
}

double Value::Get() const
{
	return m_data;
}

void Value::Print(std::ostream& os) const
{
	if (IsDefined())
	{
		os << std::fixed << std::setprecision(2) << m_data;
	}
	else
	{
		os << "nan";
	}
}

Value Value::Add(const Value& lhs, const Value& rhs)
{
	if (!lhs.IsDefined() || !rhs.IsDefined())
	{
		return Nan();
	}
	return Value(lhs.Get() + rhs.Get());
}

Value Value::Sub(const Value& lhs, const Value& rhs)
{
	if (!lhs.IsDefined() || !rhs.IsDefined())
	{
		return Nan();
	}
	return Value(lhs.Get() - rhs.Get());
}

Value Value::Mul(const Value& lhs, const Value& rhs)
{
	if (!lhs.IsDefined() || !rhs.IsDefined())
	{
		return Nan();
	}
	return Value(lhs.Get() * rhs.Get());
}

Value Value::Div(const Value& lhs, const Value& rhs)
{
	if (!lhs.IsDefined() || !rhs.IsDefined())
	{
		return Nan();
	}

	if (rhs.Get() == 0.0)
	{
		return Nan();
	}

	return Value(lhs.Get() / rhs.Get());
}

} // namespace calc