#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "CDate.h"
#include <sstream>
#include <string>

// Helper
CDate MakeInvalidDate()
{
	return { 32, Month::JANUARY, 2000 };
}

TEST_CASE("CDate Default Constructor", "[construction]")
{
	CDate date;
	REQUIRE(date.IsValid());
	REQUIRE(date.GetDay() == 1);
	REQUIRE(date.GetMonth() == Month::JANUARY);
	REQUIRE(date.GetYear() == 1970);
}

TEST_CASE("CDate Timestamp Constructor", "[construction]")
{
	SECTION("Epoch start")
	{
		CDate date(0);
		REQUIRE(date.IsValid());
		REQUIRE(date.GetDay() == 1);
		REQUIRE(date.GetMonth() == Month::JANUARY);
		REQUIRE(date.GetYear() == 1970);
	}

	SECTION("Specific date from prompt: 3 Jan 1970")
	{
		CDate date(2);
		REQUIRE(date.IsValid());
		REQUIRE(date.GetDay() == 3);
		REQUIRE(date.GetMonth() == Month::JANUARY);
		REQUIRE(date.GetYear() == 1970);
	}

	SECTION("Specific date from prompt: 2 Feb 1970")
	{
		CDate date(32);
		REQUIRE(date.IsValid());
		REQUIRE(date.GetDay() == 2);
		REQUIRE(date.GetMonth() == Month::FEBRUARY);
		REQUIRE(date.GetYear() == 1970);
	}

	SECTION("Invalid timestamp (future overflow)")
	{
		CDate date(10'000'000);
		REQUIRE_FALSE(date.IsValid());
	}
}

TEST_CASE("CDate DMY Constructor", "[construction]")
{
	SECTION("Valid date")
	{
		CDate date(25, Month::DECEMBER, 2023);
		REQUIRE(date.IsValid());
		REQUIRE(date.GetDay() == 25);
		REQUIRE(date.GetMonth() == Month::DECEMBER);
		REQUIRE(date.GetYear() == 2023);
	}

	SECTION("Leap year: 29 Feb 2020")
	{
		CDate date(29, Month::FEBRUARY, 2020);
		REQUIRE(date.IsValid());
		REQUIRE(date.GetDay() == 29);
	}

	SECTION("Non-leap year: 29 Feb 2023 -> Invalid")
	{
		CDate date(29, Month::FEBRUARY, 2023);
		REQUIRE_FALSE(date.IsValid());
	}

	SECTION("Invalid day: 0")
	{
		CDate date(0, Month::JANUARY, 2000);
		REQUIRE_FALSE(date.IsValid());
	}

	SECTION("Invalid day: 32 in Jan")
	{
		CDate date(32, Month::JANUARY, 2000);
		REQUIRE_FALSE(date.IsValid());
	}

	SECTION("Boundary: 31 Dec 9999")
	{
		CDate date(31, Month::DECEMBER, 9999);
		REQUIRE(date.IsValid());
	}

	SECTION("Boundary: 1 Jan 1970")
	{
		CDate date(1, Month::JANUARY, 1970);
		REQUIRE(date.IsValid());
	}

	SECTION("Out of range year: 1969")
	{
		CDate date(1, Month::JANUARY, 1969);
		REQUIRE_FALSE(date.IsValid());
	}

	SECTION("Out of range year: 10000")
	{
		CDate date(1, Month::JANUARY, 10000);
		REQUIRE_FALSE(date.IsValid());
	}
}

TEST_CASE("CDate GetWeekDay", "[getters]")
{
	CDate epoch(1, Month::JANUARY, 1970);
	REQUIRE(epoch.GetWeekDay() == WeekDay::THURSDAY);

	CDate nextDay = epoch + 1;
	REQUIRE(nextDay.GetWeekDay() == WeekDay::FRIDAY);

	CDate nextNextDay = epoch + 2;
	REQUIRE(nextNextDay.GetWeekDay() == WeekDay::SATURDAY);

	CDate sunday = epoch + 3;
	REQUIRE(sunday.GetWeekDay() == WeekDay::SUNDAY);
}

TEST_CASE("CDate Getters on Invalid Date", "[getters]")
{
	CDate invalid = MakeInvalidDate();
	REQUIRE_FALSE(invalid.IsValid());
	REQUIRE(invalid.GetDay() == 0);
	REQUIRE(invalid.GetYear() == 0);
}

TEST_CASE("CDate Increment/Decrement", "[arithmetic]")
{
	SECTION("Prefix ++")
	{
		CDate date(31, Month::DECEMBER, 1999);
		++date;
		REQUIRE(date.GetDay() == 1);
		REQUIRE(date.GetMonth() == Month::JANUARY);
		REQUIRE(date.GetYear() == 2000);
	}

	SECTION("Postfix ++")
	{
		CDate date(31, Month::DECEMBER, 1999);
		CDate old = date++;

		REQUIRE(old.GetYear() == 1999);
		REQUIRE(date.GetYear() == 2000);
	}

	SECTION("Prefix --")
	{
		CDate date(1, Month::JANUARY, 2000);
		--date;
		REQUIRE(date.GetDay() == 31);
		REQUIRE(date.GetMonth() == Month::DECEMBER);
		REQUIRE(date.GetYear() == 1999);
	}

	SECTION("Underflow to Invalid")
	{
		CDate date(1, Month::JANUARY, 1970);
		--date;
		REQUIRE_FALSE(date.IsValid());
	}

	SECTION("Overflow to Invalid")
	{
		CDate date(31, Month::DECEMBER, 9999);
		++date;
		REQUIRE_FALSE(date.IsValid());
	}

	SECTION("Operations on Invalid Date do not change it")
	{
		CDate date = MakeInvalidDate();
		++date;
		REQUIRE_FALSE(date.IsValid());

		--date;
		REQUIRE_FALSE(date.IsValid());

		date += 10;
		REQUIRE_FALSE(date.IsValid());
	}
}

TEST_CASE("CDate Addition/Subtraction with Days", "[arithmetic]")
{
	SECTION("Add days crossing month")
	{
		CDate date(28, Month::FEBRUARY, 2010);
		CDate result = date + 3;

		REQUIRE(result.GetDay() == 3);
		REQUIRE(result.GetMonth() == Month::MARCH);
		REQUIRE(result.GetYear() == 2010);
	}

	SECTION("Subtract days crossing year")
	{
		CDate date(1, Month::JANUARY, 2010);
		CDate result = date - 2;

		REQUIRE(result.GetDay() == 30);
		REQUIRE(result.GetMonth() == Month::DECEMBER);
		REQUIRE(result.GetYear() == 2009);
	}

	SECTION("Symmetric addition")
	{
		CDate date(1, Month::JANUARY, 2000);
		CDate res1 = date + 10;
		CDate res2 = 10 + date;

		REQUIRE(res1 == res2);
	}

	SECTION("Result out of range becomes Invalid")
	{
		CDate date(31, Month::DECEMBER, 9999);
		CDate result = date + 1;
		REQUIRE_FALSE(result.IsValid());
	}
}

TEST_CASE("CDate Difference between Dates", "[arithmetic]")
{
	SECTION("Positive difference")
	{
		CDate d1(3, Month::JANUARY, 1970);
		CDate d2(1, Month::JANUARY, 1970);
		REQUIRE((d1 - d2) == 2);
	}

	SECTION("Negative difference")
	{
		CDate d1(1, Month::JANUARY, 2010);
		CDate d2(3, Month::JANUARY, 2010);
		REQUIRE((d1 - d2) == -2);
	}

	SECTION("Same date")
	{
		CDate d1(15, Month::MAY, 2000);
		CDate d2(15, Month::MAY, 2000);
		REQUIRE((d1 - d2) == 0);
	}

	SECTION("Difference with Invalid date")
	{
		CDate valid(1, Month::JANUARY, 2000);
		CDate invalid = MakeInvalidDate();
		REQUIRE((valid - invalid) == 0);
		REQUIRE((invalid - valid) == 0);
	}
}

TEST_CASE("CDate Compound Assignment", "[arithmetic]")
{
	CDate date(1, Month::JANUARY, 2000);

	date += 31;
	REQUIRE(date.GetMonth() == Month::FEBRUARY);
	REQUIRE(date.GetDay() == 1);

	date -= 1;
	REQUIRE(date.GetDay() == 31);
	REQUIRE(date.GetMonth() == Month::JANUARY);
}

TEST_CASE("CDate Comparisons", "[comparison]")
{
	CDate d1(1, Month::JANUARY, 2000);
	CDate d2(2, Month::JANUARY, 2000);
	CDate d3(1, Month::JANUARY, 2000);

	SECTION("Equality")
	{
		REQUIRE(d1 == d3);
		REQUIRE_FALSE(d1 == d2);
	}

	SECTION("Inequality")
	{
		REQUIRE(d1 != d2);
		REQUIRE_FALSE(d1 != d3);
	}

	SECTION("Less/Greater")
	{
		REQUIRE(d1 < d2);
		REQUIRE(d2 > d1);
		REQUIRE_FALSE(d1 < d3);
		REQUIRE_FALSE(d1 > d3);
	}

	SECTION("LessEq/GreaterEq")
	{
		REQUIRE(d1 <= d3);
		REQUIRE(d1 >= d3);
		REQUIRE(d1 <= d2);
		REQUIRE(d2 >= d1);
	}

	SECTION("Comparison with Invalid")
	{
		CDate invalid = MakeInvalidDate();
		CDate valid(1, Month::JANUARY, 2000);
		REQUIRE_FALSE(valid == invalid);
		REQUIRE_FALSE(valid < invalid);
		REQUIRE_FALSE(valid > invalid);
	}
}

TEST_CASE("CDate Output Stream", "[io]")
{
	SECTION("Valid date formatting")
	{
		CDate date(5, Month::MARCH, 2021);
		std::ostringstream oss;
		oss << date;
		REQUIRE(oss.str() == "05.03.2021");
	}

	SECTION("Single digit day/month padding")
	{
		CDate date(1, Month::JANUARY, 1970);
		std::ostringstream oss;
		oss << date;
		REQUIRE(oss.str() == "01.01.1970");
	}

	SECTION("Invalid date output")
	{
		CDate date = MakeInvalidDate();
		std::ostringstream oss;
		oss << date;
		REQUIRE(oss.str() == "INVALID");
	}
}

TEST_CASE("CDate Input Stream", "[io]")
{
	SECTION("Valid input")
	{
		std::istringstream iss("15.08.1990");
		CDate date;
		iss >> date;

		REQUIRE(!iss.fail());
		REQUIRE(date.IsValid());
		REQUIRE(date.GetDay() == 15);
		REQUIRE(date.GetMonth() == Month::AUGUST);
		REQUIRE(date.GetYear() == 1990);
	}

	SECTION("Invalid format")
	{
		std::istringstream iss("15-08-1990");
		CDate date(1, Month::JANUARY, 2000);
		iss >> date;
		REQUIRE(iss.fail());
	}

	SECTION("Input INVALID string")
	{
		std::istringstream iss("INVALID");
		CDate date;
		iss >> date;

		REQUIRE(!iss.fail());
		REQUIRE_FALSE(date.IsValid());
	}

	SECTION("Input logically invalid date (32nd day)")
	{
		std::istringstream iss("32.01.2000");
		CDate date;
		iss >> date;

		REQUIRE(iss.fail());
		REQUIRE_FALSE(date.IsValid());
	}
}