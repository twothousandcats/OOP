#pragma once

#include <optional>
#include <string>

// Month
enum class Month
{
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

// WeekDay
enum class WeekDay
{
	SUNDAY = 0,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
};

// todo: либо валидный, либо не используем
class CDate
{
public:
	// DMY
	CDate(unsigned day, Month month, unsigned year);

	// Timestamp
	explicit CDate(unsigned timestamp);

	// default
	CDate();

	unsigned GetDay() const;

	Month GetMonth() const;

	unsigned GetYear() const;

	WeekDay GetWeekDay() const;

	bool IsValid() const;

	// operators
	CDate& operator+=(int days);

	CDate& operator-=(int days);

	CDate& operator++();

	CDate operator++(int);

	CDate& operator--();

	CDate operator--(int);

	// comparison
	bool operator==(const CDate& other) const;

	bool operator!=(const CDate& other) const;

	bool operator<(const CDate& other) const;

	bool operator>(const CDate& other) const;

	bool operator<=(const CDate& other) const;

	bool operator>=(const CDate& other) const;

	// friend operators for symmetric arithmetic and IO
	friend CDate operator+(CDate date, int days);

	friend CDate operator+(int days, CDate date);

	friend CDate operator-(CDate date, int days);

	friend int operator-(const CDate& lhs, const CDate& rhs);

	friend std::ostream& operator<<(std::ostream& os, const CDate& date);

	friend std::istream& operator>>(std::istream& is, CDate& date);

private:
	// days since unix epoch
	std::optional<unsigned> m_daysSinceEpoch;

	// Convert days since epoch to YMD
	struct Ymd
	{
		unsigned day;
		Month month;
		unsigned year;
	};

	// Check if days count is within valid range
	static bool IsDaysValid(unsigned days);

	// Convert YMD to days since epoch
	static unsigned YmdToDays(unsigned day, Month month, unsigned year);

	static Ymd DaysToYmd(unsigned days);

	// Check if year is leap
	static bool IsLeapYear(unsigned year);

	// get days in month
	static unsigned GetDaysInMonth(Month month, unsigned year);
};