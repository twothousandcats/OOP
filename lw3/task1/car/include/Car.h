#pragma once
enum class Direction
{
	StandingStill,
	Forward,
	Backward
};

class Car
{
public:
	// setters
	bool TurnOnEngine();

	bool TurnOffEngine();

	bool SetGear(int gear);

	bool SetSpeed(int speed);

	// getters
	bool IsTurnedOn() const;

	Direction GetDirection() const;

	int GetSpeed() const;

	int GetGear() const;

private:
	bool m_engineOn{ false };
	int m_gear{ 0 }; // -1 to 5
	int m_speed{ 0 }; // non-negative
	Direction m_lastDirection{ Direction::StandingStill }; // todo; rename smth like "inertia"

	struct GearRange
	{
		int minSpeed;
		int maxSpeed;
	};

	static constexpr GearRange GEAR_RANGES[7] = {
		{ 0, 20 }, // -1
		{ 0, 0 }, // n
		{ 0, 30 }, // 1
		{ 20, 50 }, // 2
		{ 30, 60 }, // 3
		{ 40, 90 }, // 4
		{ 50, 150 } // 5
	};

	bool IsValidGear(int gear) const;

	bool CanChangeGear(int newGear) const;

	bool IsSpeedInRangeForGear(int speed, int gear) const;

	Direction CalculateDirection() const;
};