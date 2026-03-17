#include "Car.h"

constexpr int MIN_GEAR = -1;
constexpr int MAX_GEAR = 5;

bool Car::IsTurnedOn() const
{
	return m_engineOn;
}

Direction Car::GetDirection() const
{
	return CalculateDirection();
}

int Car::GetSpeed() const
{
	return m_speed;
}

int Car::GetGear() const
{
	return m_gear;
}

bool Car::TurnOnEngine()
{
	if (m_engineOn)
	{
		return true;
	}

	if (m_gear == 0 && m_speed == 0)
	{
		m_engineOn = true;
		return true;
	}

	return false;
}

bool Car::TurnOffEngine()
{
	if (!m_engineOn)
	{
		return true;
	}

	if (m_gear == 0 && m_speed == 0)
	{
		m_engineOn = false;
		return true;
	}

	return false;
}

bool Car::SetGear(const int gear)
{
	if (!IsValidGear(gear))
	{
		return false;
	}

	if (!m_engineOn)
	{
		if (gear != 0)
		{
			return false;
		}
		m_gear = gear;
		return true;
	}

	if (!CanChangeGear(gear))
	{
		return false;
	}

	m_gear = gear;
	return true;
}

bool Car::SetSpeed(const int speed)
{
	if (speed < 0)
	{
		return false;
	}

	if (!m_engineOn)
	{
		return false;
	}

	if (m_gear == 0 && speed > m_speed)
	{
		return false;
	}

	if (!IsSpeedInRangeForGear(speed, m_gear))
	{
		return false;
	}

	m_speed = speed;

	// inertia accounting
	if (m_speed > 0)
	{
		if (m_gear == -1)
		{
			m_lastDirection = Direction::Backward;
		}
		else if (m_gear > 0)
		{
			m_lastDirection = Direction::Forward;
		}
	}

	return true;
}

bool Car::IsValidGear(const int gear) const
{
	return gear >= MIN_GEAR && gear <= MAX_GEAR;
}

bool Car::CanChangeGear(const int newGear) const
{
	if (newGear == m_gear)
	{
		return true;
	}

	if (newGear == -1 && m_speed != 0)
	{
		return false;
	}

	if (m_gear == -1 && m_speed != 0 && newGear != 0)
	{
		return false;
	}

	if (m_speed > 0)
	{
		if ((m_gear < 0 && newGear > 0) || (m_gear > 0 && newGear < 0))
		{
			return false;
		}
	}

	if (!IsSpeedInRangeForGear(m_speed, newGear))
	{
		return false;
	}

	return true;
}

bool Car::IsSpeedInRangeForGear(const int speed, const int gear) const
{
	if (gear == 0)
	{
		return true;
	}

	const auto& [minSpeed, maxSpeed] = GEAR_RANGES[gear + 1];
	return speed >= minSpeed && speed <= maxSpeed;
}

Direction Car::CalculateDirection() const
{
	if (m_speed == 0)
	{
		return Direction::StandingStill;
	}

	if (m_gear == -1)
	{
		return Direction::Backward;
	}

	if (m_gear > 0)
	{
		if (m_lastDirection == Direction::Backward)
		{
			return Direction::Backward;
		}
		return Direction::Forward;
	}

	return m_lastDirection;
}