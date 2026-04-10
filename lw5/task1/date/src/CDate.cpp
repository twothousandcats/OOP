#include "CDate.h"
#include <array>
#include <stdexcept>
#include <iostream>

namespace
{
// todo: доработать константы
constexpr unsigned MIN_YEAR = 1970;
constexpr unsigned MAX_YEAR = 9999;
constexpr unsigned DAYS_IN_WEEK = 7;
constexpr unsigned DAYS_IN_YEAR = 365;
constexpr unsigned DAYS_IN_400_YEARS = 400 * DAYS_IN_YEAR + 97; // 400 * 365 + 97 leap days
constexpr unsigned DAYS_IN_100_YEARS = 100 * DAYS_IN_YEAR + 24; // 100 * 365 + 24 leap days (first 400-years cycle is different)
constexpr unsigned DAYS_IN_4_YEARS = 4 * DAYS_IN_YEAR + 1; // 4 * 365 + 1 leap day
constexpr auto EPOCH_WEEKDAY = WeekDay::THURSDAY;
constexpr std::array<unsigned, 13> DAYS_IN_MONTH_NORMAL = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

constexpr std::array<unsigned, 13> DAYS_IN_MONTH_LEAP = {
	0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
}

bool CDate::IsLeapYear(const unsigned year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

unsigned CDate::GetDaysInMonth(Month month, unsigned year)
{
	const auto idx = static_cast<unsigned>(month);
	if (idx < 1 || idx > 12)
	{
		return 0;
	}

	if (IsLeapYear(year))
	{
		return DAYS_IN_MONTH_LEAP[idx];
	}

	return DAYS_IN_MONTH_NORMAL[idx];
}

unsigned CDate::YmdToDays(const unsigned day, Month month, const unsigned year)
{
	if (year < MIN_YEAR || year > MAX_YEAR)
	{
		throw std::out_of_range("Year out of range");
	}

	unsigned monthIdx = static_cast<unsigned>(month);
	if (monthIdx < 1 || monthIdx > 12)
	{
		throw std::invalid_argument("Invalid month");
	}

	if (day < 1 || day > GetDaysInMonth(month, year))
	{
		throw std::invalid_argument("Invalid day");
	}

	const unsigned yearsSinceEpoch = year - MIN_YEAR;

	const auto countLeapYears = [](unsigned y) -> unsigned {
		return y / 4 - y / 100 + y / 400;
	};

	constexpr unsigned leapYearsBeforeEpoch = countLeapYears(MIN_YEAR - 1);
	const unsigned leapYearsBeforeYear = countLeapYears(year - 1);
	const unsigned leapYearsInRange = leapYearsBeforeYear - leapYearsBeforeEpoch;

	const unsigned normalYearsInRange = yearsSinceEpoch - leapYearsInRange;

	unsigned totalDays = normalYearsInRange * 365 + leapYearsInRange * 366;

	for (unsigned m = 1; m < monthIdx; ++m)
	{
		totalDays += GetDaysInMonth(static_cast<Month>(m), year);
	}

	totalDays += (day - 1);

	return totalDays;
}

CDate::Ymd CDate::DaysToYmd(unsigned days)
{

	// 400-year cycle
	unsigned remainingDays = days;
	const unsigned cycles400 = remainingDays / DAYS_IN_400_YEARS;
	unsigned year = MIN_YEAR + cycles400 * 400;
	remainingDays %= DAYS_IN_400_YEARS;

	// 100-year cycle
	unsigned cycles100 = remainingDays / DAYS_IN_100_YEARS;
	if (cycles100 == 4)
	{
		cycles100 = 3;
	}
	year += cycles100 * 100;
	remainingDays -= cycles100 * DAYS_IN_100_YEARS;

	// 4-year cycle
	const unsigned cycles4 = remainingDays / DAYS_IN_4_YEARS;
	year += cycles4 * 4;
	remainingDays %= DAYS_IN_4_YEARS;

	unsigned singleYears = remainingDays / DAYS_IN_YEAR;
	if (singleYears >= 4)
	{
		singleYears = 3;
	}

	unsigned daysInSingleYears = 0;
	for (unsigned i = 0; i < singleYears; ++i)
	{
		daysInSingleYears += IsLeapYear(year + i) ? 366 : 365;
	}

	if (daysInSingleYears <= remainingDays)
	{
		year += singleYears;
		remainingDays -= daysInSingleYears;
	}
	else
	{
		for (unsigned i = 0; i < singleYears; ++i)
		{
			const unsigned daysInThisYear = IsLeapYear(year)
				? 366
				: 365;
			if (remainingDays < daysInThisYear)
			{
				break;
			}
			remainingDays -= daysInThisYear;
			++year;
		}
	}

	unsigned monthIdx = 1;
	while (monthIdx <= 12)
	{
		unsigned daysInCurrentMonth = GetDaysInMonth(static_cast<Month>(monthIdx), year);
		if (remainingDays < daysInCurrentMonth)
		{
			break;
		}

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
		std::string token;
		if (is >> token)
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
		else
		{
			is.setstate(std::ios::failbit);
		}
	}
	return is;
}