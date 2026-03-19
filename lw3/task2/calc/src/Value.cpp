#include "Value.h"

namespace calc {

Value::Value() : m_data(std::numeric_limits<double>::quiet_NaN()) {}

Value::Value(double val) : m_data(val) {}

Value Value::nan() {
	return Value(std::numeric_limits<double>::quiet_NaN());
}

bool Value::isDefined() const {
	return !std::isnan(m_data);
}

double Value::get() const {
	return m_data;
}

void Value::print(std::ostream& os) const {
	if (isDefined()) {
		os << std::fixed << std::setprecision(2) << m_data;
	} else {
		os << "nan";
	}
}

std::ostream& operator<<(std::ostream& os, const Value& v) {
	v.print(os);
	return os;
}

Value Value::add(const Value& lhs, const Value& rhs) {
	if (!lhs.isDefined() || !rhs.isDefined()) return nan();
	return Value(lhs.get() + rhs.get());
}

Value Value::sub(const Value& lhs, const Value& rhs) {
	if (!lhs.isDefined() || !rhs.isDefined()) return nan();
	return Value(lhs.get() - rhs.get());
}

Value Value::mul(const Value& lhs, const Value& rhs) {
	if (!lhs.isDefined() || !rhs.isDefined()) return nan();
	return Value(lhs.get() * rhs.get());
}

Value Value::div(const Value& lhs, const Value& rhs) {
	if (!lhs.isDefined() || !rhs.isDefined()) return nan();
	// Деление на ноль дает inf или nan в IEEE754, но по требованию: "значение считается неопределенным (nan)"
	if (rhs.get() == 0.0) return nan();
	return Value(lhs.get() / rhs.get());
}

} // namespace calc