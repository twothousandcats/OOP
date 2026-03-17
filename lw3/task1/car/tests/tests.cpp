#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "Car.h"

#define CHECK_CAR_STATE(car, expEngine, expGear, expSpeed, expDir) \
CHECK((car).IsTurnedOn() == (expEngine));\
CHECK((car).GetGear() == (expGear));\
CHECK((car).GetSpeed() == (expSpeed));\
CHECK((car).GetDirection() == (expDir));

TEST_CASE("Car initial state", "[init]")
{
	Car car;
	CHECK_CAR_STATE(car, false, 0, 0, Direction::StandingStill);
}

TEST_CASE("Engine management", "[engine]")
{
	Car car;

	SECTION("Turn on engine twice")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.TurnOnEngine());
		CHECK(car.IsTurnedOn());
	}

	SECTION("Cannot turn off engine while moving")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(10));

		CHECK_FALSE(car.TurnOffEngine());
		CHECK(car.IsTurnedOn());
	}

	SECTION("Cannot turn off engine not in neutral")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.SetGear(1));

		CHECK_FALSE(car.TurnOffEngine());
		CHECK(car.IsTurnedOn());
	}

	SECTION("Turn off engine correctly")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.TurnOffEngine());
		CHECK_FALSE(car.IsTurnedOn());
		CHECK_CAR_STATE(car, false, 0, 0, Direction::StandingStill);
	}
}

TEST_CASE("Gear shifting logic", "[gear]")
{
	Car car;
	SECTION("Cannot set gear while engine is off")
	{
		CHECK_FALSE(car.SetGear(1));
		CHECK(car.GetGear() == 0);
	}

	SECTION("Can set neutral while engine is off")
	{
		CHECK(car.SetGear(0));
	}

	SECTION("Valid gear shifts")
	{
		REQUIRE(car.TurnOnEngine());
		CHECK(car.SetGear(1));
		CHECK(car.GetGear() == 1);

		CHECK(car.SetSpeed(20));
		CHECK(car.SetGear(2));
		CHECK(car.GetGear() == 2);
	}

	SECTION("Cannot shift to reverse while moving")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(10));

		CHECK_FALSE(car.SetGear(-1));
		CHECK(car.GetGear() == 1);
	}

	SECTION("Cannot shift gear if speed out of new gear range")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(25));
		CHECK(car.SetGear(2));

		CHECK_FALSE(car.SetSpeed(55));
		REQUIRE(car.SetSpeed(40));

		CHECK(car.SetGear(3));

		CHECK_FALSE(car.SetGear(5));
		CHECK(car.GetGear() == 3);
	}
}

TEST_CASE("Speed management", "[speed]")
{
	Car car;
	SECTION("Cannot set speed while engine is off")
	{
		CHECK_FALSE(car.SetSpeed(10));
		CHECK(car.GetSpeed() == 0);
	}

	SECTION("Cannot set negative speed")
	{
		REQUIRE(car.TurnOnEngine());
		CHECK_FALSE(car.SetSpeed(-5));
		CHECK(car.GetSpeed() == 0);
	}

	SECTION("Accelerate and brake in gear")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.SetGear(1));

		REQUIRE(car.SetSpeed(10));
		CHECK(car.GetSpeed() == 10);

		REQUIRE(car.SetSpeed(20));
		CHECK(car.GetSpeed() == 20);

		REQUIRE(car.SetSpeed(5));
		CHECK(car.GetSpeed() == 5);
	}

	SECTION("Cannot accelerate on neutral")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(20));

		REQUIRE(car.SetGear(0));
		CHECK(car.GetGear() == 0);

		CHECK_FALSE(car.SetSpeed(25));
		CHECK(car.GetSpeed() == 20);

		CHECK(car.SetSpeed(10));
		CHECK(car.GetSpeed() == 10);
	}

	SECTION("Speed limits per gear")
	{
		REQUIRE(car.TurnOnEngine());
		REQUIRE(car.SetGear(1));

		CHECK_FALSE(car.SetSpeed(31));
		CHECK(car.SetSpeed(30));

		REQUIRE(car.SetGear(2));
		CHECK_FALSE(car.SetSpeed(19));
		CHECK_FALSE(car.SetSpeed(51));
		CHECK(car.SetSpeed(40));
	}
}

TEST_CASE("Reverse gear specific rules", "[reverse]")
{
	Car car;
	REQUIRE(car.TurnOnEngine());
	SECTION("Reverse only at zero speed")
	{
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(1));
		CHECK_FALSE(car.SetGear(-1));

		REQUIRE(car.SetSpeed(0));
		CHECK(car.SetGear(-1));
		CHECK(car.GetGear() == -1);
	}

	SECTION("From reverse to forward only at zero speed")
	{
		REQUIRE(car.SetGear(-1));
		REQUIRE(car.SetSpeed(10));

		CHECK_FALSE(car.SetGear(1));
		CHECK(car.GetGear() == -1);

		REQUIRE(car.SetSpeed(0));
		CHECK(car.SetGear(1));
		CHECK(car.GetGear() == 1);
	}

	SECTION("Reverse -> Neutral -> Forward prohibition (The tricky case)")
	{
		REQUIRE(car.SetGear(-1));
		REQUIRE(car.SetSpeed(10));

		REQUIRE(car.SetGear(0));
		CHECK(car.GetGear() == 0);
		CHECK(car.GetSpeed() == 10);
		CHECK(car.GetDirection() == Direction::Backward);

		CHECK(car.SetGear(1));
		CHECK(car.GetGear() == 1);

		REQUIRE(car.SetSpeed(0));
		CHECK(car.GetDirection() == Direction::StandingStill);

		CHECK(car.SetGear(1));
		CHECK(car.GetGear() == 1);
	}
}

TEST_CASE("Direction calculation", "[direction]")
{
	Car car;
	REQUIRE(car.TurnOnEngine());
	SECTION("Standing still")
	{
		CHECK(car.GetDirection() == Direction::StandingStill);
		REQUIRE(car.SetGear(1));
		CHECK(car.GetDirection() == Direction::StandingStill);
	}

	SECTION("Forward")
	{
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(10));
		CHECK(car.GetDirection() == Direction::Forward);
	}

	SECTION("Backward")
	{
		REQUIRE(car.SetGear(-1));
		REQUIRE(car.SetSpeed(10));
		CHECK(car.GetDirection() == Direction::Backward);
	}

	SECTION("Coasting backward on neutral")
	{
		REQUIRE(car.SetGear(-1));
		REQUIRE(car.SetSpeed(20));

		CHECK(car.GetSpeed() == 20);
		CHECK(car.GetDirection() == Direction::Backward);
	}

	SECTION("Coasting forward on neutral")
	{
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(30));
		REQUIRE(car.SetGear(2));
		REQUIRE(car.SetSpeed(40));
		REQUIRE(car.SetGear(0));

		CHECK(car.GetSpeed() == 40);
		CHECK(car.GetDirection() == Direction::Forward);
	}

	// добавил тест-кейс, сломавшим программу при сдаче
	SECTION("Dir after -1 -> 0 -> 1")
	{
		CHECK(car.GetDirection() == Direction::StandingStill);
		REQUIRE(car.SetGear(-1));
		REQUIRE(car.SetSpeed(20));
		CHECK(car.GetDirection() == Direction::Backward);
		REQUIRE(car.SetSpeed(10));
		CHECK(car.GetDirection() == Direction::Backward);
		REQUIRE(car.SetGear(0));
		CHECK(car.GetDirection() == Direction::Backward);
		REQUIRE(car.SetGear(1));
		CHECK(car.GetDirection() == Direction::Backward);
	}
}

TEST_CASE("Edge cases and boundaries", "[boundary]")
{
	Car car;
	REQUIRE(car.TurnOnEngine());
	SECTION("Max speed in 5th gear")
	{
		REQUIRE(car.SetGear(5) == false);
		REQUIRE(car.SetGear(1));
		REQUIRE(car.SetSpeed(20));
		REQUIRE(car.SetGear(2));
		REQUIRE(car.SetSpeed(40));
		REQUIRE(car.SetGear(3));
		REQUIRE(car.SetSpeed(60));
		REQUIRE(car.SetGear(4));
		REQUIRE(car.SetSpeed(90));
		REQUIRE(car.SetGear(5));

		CHECK(car.SetSpeed(150));
		CHECK(car.GetSpeed() == 150);

		CHECK_FALSE(car.SetSpeed(151));
	}

	SECTION("Invalid gear values")
	{
		CHECK_FALSE(car.SetGear(6));
		CHECK_FALSE(car.SetGear(-2));
		CHECK_FALSE(car.SetGear(100));
	}
}