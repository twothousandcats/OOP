#include "CDate.h"
#include <array>
#include <stdexcept>
#include <iostream>

namespace
{
// range
constexpr unsigned MIN_YEAR = 1970;
constexpr unsigned MAX_YEAR = 9999;

// Time unit constants
constexpr unsigned DAYS_IN_WEEK = 7;
constexpr unsigned DAYS_IN_NORMAL_YEAR = 365;
constexpr unsigned DAYS_IN_LEAP_YEAR = 366;
constexpr unsigned MONTHS_IN_YEAR = 12;

// Leap year cycle constants
constexpr unsigned LEAP_CYCLE_4_YEARS = 4;
constexpr unsigned LEAP_CYCLE_100_YEARS = 100;
constexpr unsigned LEAP_CYCLE_400_YEARS = 400;

// Days in leap year cycles
constexpr unsigned LEAP_DAYS_IN_400_YEARS = 97;
constexpr unsigned LEAP_DAYS_IN_100_YEARS = 24;
constexpr unsigned LEAP_DAYS_IN_4_YEARS = 1;

// Total days in leap year cycles
constexpr unsigned DAYS_IN_400_YEARS = LEAP_CYCLE_400_YEARS * DAYS_IN_NORMAL_YEAR + LEAP_DAYS_IN_400_YEARS;
constexpr unsigned DAYS_IN_100_YEARS = LEAP_CYCLE_100_YEARS * DAYS_IN_NORMAL_YEAR + LEAP_DAYS_IN_100_YEARS;
constexpr unsigned DAYS_IN_4_YEARS = LEAP_CYCLE_4_YEARS * DAYS_IN_NORMAL_YEAR + LEAP_DAYS_IN_4_YEARS;

// Epoch reference
constexpr auto EPOCH_WEEKDAY = WeekDay::THURSDAY;

// Days in months (0 is unused, indix 1-12 correspond to Month enum values)
constexpr std::array<unsigned, 13> DAYS_IN_MONTH_NORMAL = {
	0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
constexpr std::array<unsigned, 13> DAYS_IN_MONTH_LEAP = {
	0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
}

// constructs
CDate::CDate()
	: m_daysSinceEpoch(0) // 1 Jan 1970
{
}

// todo: использовать делегирующий construct, основа - timestamp
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

bool CDate::IsLeapYear(const unsigned year)
{
	return (year % LEAP_CYCLE_4_YEARS == 0 && year % LEAP_CYCLE_100_YEARS != 0) ||
		(year % LEAP_CYCLE_400_YEARS == 0);
}

unsigned CDate::GetDaysInMonth(Month month, const unsigned year)
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

	const auto monthIdx = static_cast<unsigned>(month);
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
		return y / LEAP_CYCLE_4_YEARS - y / LEAP_CYCLE_100_YEARS + y / LEAP_CYCLE_400_YEARS;
	};

	constexpr unsigned leapYearsBeforeEpoch = countLeapYears(MIN_YEAR - 1);
	const unsigned leapYearsBeforeYear = countLeapYears(year - 1);
	const unsigned leapYearsInRange = leapYearsBeforeYear - leapYearsBeforeEpoch;
	const unsigned normalYearsInRange = yearsSinceEpoch - leapYearsInRange;

	unsigned totalDays = normalYearsInRange * DAYS_IN_NORMAL_YEAR + leapYearsInRange * DAYS_IN_LEAP_YEAR;

	for (unsigned m = 1; m < monthIdx; ++m)
	{
		totalDays += GetDaysInMonth(static_cast<Month>(m), year);
	}

	totalDays += (day - 1);

	return totalDays;
}

// convert
CDate::Ymd CDate::DaysToYmd(const unsigned days)
{
	// 400-year cycle
	// idk how to rename
	unsigned remainingDays = days;
	const unsigned cycles400 = remainingDays / DAYS_IN_400_YEARS;
	unsigned year = MIN_YEAR + cycles400 * LEAP_CYCLE_400_YEARS;
	remainingDays %= DAYS_IN_400_YEARS;

	// 100-year cycle
	unsigned cycles100 = remainingDays / DAYS_IN_100_YEARS;
	if (cycles100 == LEAP_CYCLE_4_YEARS)
	{
		cycles100 = LEAP_CYCLE_4_YEARS - 1;
	}
	year += cycles100 * LEAP_CYCLE_100_YEARS;
	remainingDays -= cycles100 * DAYS_IN_100_YEARS;

	// 4-year cycle
	const unsigned cycles4 = remainingDays / DAYS_IN_4_YEARS;
	year += cycles4 * LEAP_CYCLE_4_YEARS;
	remainingDays %= DAYS_IN_4_YEARS; // num of day

	unsigned singleYears = remainingDays / DAYS_IN_NORMAL_YEAR;
	if (singleYears >= LEAP_CYCLE_4_YEARS)
	{
		singleYears = LEAP_CYCLE_4_YEARS - 1;
	}

	unsigned daysInSingleYears = 0;
	for (unsigned i = 0; i < singleYears; ++i)
	{
		daysInSingleYears += IsLeapYear(year + i) ? DAYS_IN_LEAP_YEAR : DAYS_IN_NORMAL_YEAR;
	}

	// determine days in current year && year
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
				? DAYS_IN_LEAP_YEAR
				: DAYS_IN_NORMAL_YEAR;
			if (remainingDays < daysInThisYear)
			{
				break;
			}
			remainingDays -= daysInThisYear;
			++year;
		}
	}

	unsigned monthIdx = 1;
	while (monthIdx <= MONTHS_IN_YEAR)
	{
		const unsigned daysInCurrentMonth = GetDaysInMonth(static_cast<Month>(monthIdx), year);
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

bool CDate::IsDaysValid(const unsigned days)
{
	try
	{
		const unsigned maxDays = YmdToDays(31, Month::DECEMBER, MAX_YEAR);
		return days <= maxDays;
	}
	catch (...)
	{
		return false;
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
	const auto ymd = DaysToYmd(*m_daysSinceEpoch);
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

	unsigned weekdayIndex = (*m_daysSinceEpoch + static_cast<unsigned>(EPOCH_WEEKDAY)) % DAYS_IN_WEEK;
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

	if (const auto newDays = static_cast<long long>(*m_daysSinceEpoch) + days; newDays < 0 || !IsDaysValid(static_cast<unsigned>(newDays)))
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
	const CDate temp = *this;
	--(*this);
	return temp;
}

// free functions
CDate operator+(CDate date, const int days)
{
	date += days;
	return date;
}

CDate operator+(const int days, CDate date)
{
	date += days;
	return date;
}

CDate operator-(CDate date, const int days)
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

	const unsigned day = date.GetDay();
	const Month month = date.GetMonth();
	const unsigned year = date.GetYear();
	if (day < 10)
	{
		os << '0';
	}
	os << day << '.';

	const auto monthNum = static_cast<unsigned>(month);
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
				if (const CDate temp(day, static_cast<Month>(monthNum), year); temp.IsValid())
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
		else
		{
			is.setstate(std::ios::failbit);
		}
	}
	return is;
}