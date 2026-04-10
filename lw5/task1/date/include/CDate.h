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

// format DD.MM.YYYY
// range: 1970 - 9999
class CDate
{
public:
	static constexpr unsigned MIN_YEAR = 1970;
	static constexpr unsigned MAX_YEAR = 9999;

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

	CDate& operator++(); // prefix ++
	CDate operator++(int); // postfix ++

	CDate& operator--(); // prefix --
	CDate operator--(int); // postfix --

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

	// Check if days count is within valid range
	static bool IsDaysValid(unsigned days);

	// Convert YMD to days since epoch
	static unsigned YmdToDays(unsigned day, Month month, unsigned year);

	// Convert days since epoch to YMD
	struct Ymd
	{
		unsigned day;
		Month month;
		unsigned year;
	};

	static Ymd DaysToYmd(unsigned days);

	// Check if year is leap
	static bool IsLeapYear(unsigned year);

	// get days in month
	static unsigned GetDaysInMonth(Month month, unsigned year);
};