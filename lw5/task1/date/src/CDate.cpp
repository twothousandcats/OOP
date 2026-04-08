#include "CDate.h"
#include <array>

namespace
{
constexpr unsigned DAYS_IN_WEEK = 7;
constexpr auto EPOCH_WEEKDAY = WeekDay::THURSDAY;
constexpr std::array<unsigned, 13> DAYS_IN_MONTH_NORMAL = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

constexpr std::array<unsigned, 13> DAYS_IN_MONTH_LEAP = {
	0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
}

bool CDate::IsLeapYear(unsigned year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

unsigned CDate::GetDaysInMonth(Month month, unsigned year)
{
	unsigned idx = static_cast<unsigned>(month);
	if (idx < 1 || idx > 12)
		return 0;

	if (IsLeapYear(year))
		return DAYS_IN_MONTH_LEAP[idx];
	else
		return DAYS_IN_MONTH_NORMAL[idx];
}

unsigned CDate::YmdToDays(unsigned day, Month month, unsigned year)
{
	if (year < MIN_YEAR || year > MAX_YEAR)
	{
		throw std::out_of_range("Year out of range");
	}

	unsigned totalDays = 0;

	for (unsigned y = MIN_YEAR; y < year; ++y)
	{
		totalDays += IsLeapYear(y) ? 366 : 365;
	}

	unsigned monthIdx = static_cast<unsigned>(month);
	if (monthIdx < 1 || monthIdx > 12)
	{
		throw std::invalid_argument("Invalid month");
	}

	for (unsigned m = 1; m < monthIdx; ++m)
	{
		totalDays += GetDaysInMonth(static_cast<Month>(m), year);
	}

	if (day < 1 || day > GetDaysInMonth(month, year))
	{
		throw std::invalid_argument("Invalid day");
	}

	totalDays += (day - 1);

	return totalDays;
}

CDate::Ymd CDate::DaysToYmd(unsigned days)
{
	unsigned year = MIN_YEAR;
	unsigned remainingDays = days;

	if (remainingDays > 365)
	{
		unsigned estimatedYears = remainingDays / 365;
		if (estimatedYears > 0)
			estimatedYears--;

		unsigned daysInEstimatedYears = 0;
		for (unsigned y = MIN_YEAR; y < MIN_YEAR + estimatedYears; ++y)
		{
			daysInEstimatedYears += IsLeapYear(y) ? 366 : 365;
		}

		if (daysInEstimatedYears <= remainingDays)
		{
			year += estimatedYears;
			remainingDays -= daysInEstimatedYears;
		}
	}

	while (true)
	{
		unsigned daysInCurrentYear = IsLeapYear(year) ? 366 : 365;
		if (remainingDays < daysInCurrentYear)
			break;

		remainingDays -= daysInCurrentYear;
		++year;
	}

	unsigned monthIdx = 1;
	while (monthIdx <= 12)
	{
		unsigned daysInCurrentMonth = GetDaysInMonth(static_cast<Month>(monthIdx), year);
		if (remainingDays < daysInCurrentMonth)
			break;

		remainingDays -= daysInCurrentMonth;
		++monthIdx;
	}

	return {
		.day = remainingDays + 1,
		.month = static_cast<Month>(monthIdx),
		.year = year
	};
}

bool CDate::IsDaysValid(unsigned days)
{
	try
	{
		unsigned maxDays = YmdToDays(31, Month::DECEMBER, MAX_YEAR);
		return days <= maxDays;
	}
	catch (...)
	{
		return false;
	}
}

// constructs

CDate::CDate()
	: m_daysSinceEpoch(0) // 1 Jan 1970
{
}

CDate::CDate(const unsigned day, const Month month, const unsigned year)
{
	try
	{
		m_daysSinceEpoch = YmdToDays(day, month, year);
		if (!IsDaysValid(*m_daysSinceEpoch))
		{
			m_daysSinceEpoch.reset();
		}
	}
	catch (...)
	{
		m_daysSinceEpoch.reset();
	}
}

CDate::CDate(unsigned timestamp)
{
	if (IsDaysValid(timestamp))
	{
		m_daysSinceEpoch = timestamp;
	}
	else
	{
		m_daysSinceEpoch.reset();
	}
}

unsigned CDate::GetDay() const
{
	if (!IsValid())
	{
		return 0;
	}
	const auto ymd = DaysToYmd(*m_daysSinceEpoch);
	return ymd.day;
}

Month CDate::GetMonth() const
{
	if (!IsValid())
	{
		return Month::JANUARY;
	}
	auto ymd = DaysToYmd(*m_daysSinceEpoch);
	return ymd.month;
}

unsigned CDate::GetYear() const
{
	if (!IsValid())
	{
		return 0;
	}
	const auto ymd = DaysToYmd(*m_daysSinceEpoch);
	return ymd.year;
}

WeekDay CDate::GetWeekDay() const
{
	if (!IsValid())
	{
		return WeekDay::SUNDAY;
	}

	const unsigned days = *m_daysSinceEpoch;
	unsigned weekdayIndex = (days + static_cast<unsigned>(EPOCH_WEEKDAY)) % DAYS_IN_WEEK;
	return static_cast<WeekDay>(weekdayIndex);
}

bool CDate::IsValid() const
{
	return m_daysSinceEpoch.has_value();
}

// arithmetic
CDate& CDate::operator+=(const int days)
{
	if (!IsValid())
	{
		return *this;
	}

	long long newDays = static_cast<long long>(*m_daysSinceEpoch) + days;
	if (newDays < 0 || !IsDaysValid(static_cast<unsigned>(newDays)))
	{
		m_daysSinceEpoch.reset();
	}
	else
	{
		m_daysSinceEpoch = static_cast<unsigned>(newDays);
	}
	return *this;
}

CDate& CDate::operator-=(const int days)
{
	return *this += (-days);
}

CDate& CDate::operator++()
{
	*this += 1;
	return *this;
}

CDate CDate::operator++(int)
{
	const CDate temp = *this;
	++(*this);
	return temp;
}

CDate& CDate::operator--()
{
	*this -= 1;
	return *this;
}

CDate CDate::operator--(int)
{
	CDate temp = *this;
	--(*this);
	return temp;
}

// free functions
CDate operator+(CDate date, int days)
{
	date += days;
	return date;
}

CDate operator+(int days, CDate date)
{
	date += days;
	return date;
}

CDate operator-(CDate date, int days)
{
	date -= days;
	return date;
}

int operator-(const CDate& lhs, const CDate& rhs)
{
	if (!lhs.IsValid() || !rhs.IsValid())
	{
		return 0;
	}
	return static_cast<int>(*lhs.m_daysSinceEpoch) - static_cast<int>(*rhs.m_daysSinceEpoch);
}

// comparison
bool CDate::operator==(const CDate& other) const
{
	if (!IsValid() || !other.IsValid())
	{
		return false;
	}
	return *m_daysSinceEpoch == *other.m_daysSinceEpoch;
}

bool CDate::operator!=(const CDate& other) const
{
	return !(*this == other);
}

bool CDate::operator<(const CDate& other) const
{
	if (!IsValid() || !other.IsValid())
	{
		return false;
	}
	return *m_daysSinceEpoch < *other.m_daysSinceEpoch;
}

bool CDate::operator>(const CDate& other) const
{
	return other < *this;
}

bool CDate::operator<=(const CDate& other) const
{
	return (*this < other) || (*this == other);
}

bool CDate::operator>=(const CDate& other) const
{
	return (*this > other) || (*this == other);
}

// IO
std::ostream& operator<<(std::ostream& os, const CDate& date)
{
	if (!date.IsValid())
	{
		os << "INVALID";
		return os;
	}

	unsigned day = date.GetDay();
	Month month = date.GetMonth();
	unsigned year = date.GetYear();
	if (day < 10)
	{
		os << '0';
	}
	os << day << '.';

	unsigned monthNum = static_cast<unsigned>(month);
	if (monthNum < 10)
	{
		os << '0';
	}
	os << monthNum << '.';

	os << year;

	return os;
}

std::istream& operator>>(std::istream& is, CDate& date)
{
	unsigned day = 0, year = 0;
	unsigned monthNum = 0;
	char dot1 = 0, dot2 = 0;

	if (is >> day >> dot1 >> monthNum >> dot2 >> year)
	{
		if (dot1 == '.' && dot2 == '.' && monthNum >= 1 && monthNum <= 12)
		{
			try
			{
				CDate temp(day, static_cast<Month>(monthNum), year);
				if (temp.IsValid())
				{
					date = temp;
				}
				else
				{
					date.m_daysSinceEpoch.reset();
					is.setstate(std::ios::failbit);
				}
			}
			catch (...)
			{
				date.m_daysSinceEpoch.reset();
				is.setstate(std::ios::failbit);
			}
		}
		else
		{
			is.setstate(std::ios::failbit);
		}
	}
	else
	{
		is.clear();
		if (std::string token; is >> token)
		{
			if (token == "INVALID")
			{
				date.m_daysSinceEpoch.reset();
			}
			else
			{
				is.setstate(std::ios::failbit);
			}
		}
	}
	return is;
}