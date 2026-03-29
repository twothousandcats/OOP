#include "car.h"
#include <iostream>
#include <sstream>
#include <string>
#include <limits>

void PrintInfo(const Car& car)
{
	std::cout << "Engine: " << (car.IsTurnedOn() ? "on" : "off") << "\n";

	switch (car.GetDirection())
	{
	case Direction::Forward:
		std::cout << "Direction: forward\n";
		break;
	case Direction::Backward:
		std::cout << "Direction: backward\n";
		break;
	case Direction::StandingStill:
		std::cout << "Direction: standing still\n";
		break;
	}

	std::cout << "Speed: " << car.GetSpeed() << "\n";
	std::cout << "Gear: " << car.GetGear() << "\n";
}

bool ParseIntArgument(const std::string& argStr, int& outValue)
{
	if (argStr.empty())
	{
		return false;
	}

	try
	{
		size_t pos = 0;
		const long long val = std::stoll(argStr, &pos);
		if (pos != argStr.length())
		{
			return false;
		}

		if (val < std::numeric_limits<int>::min() || val > std::numeric_limits<int>::max())
		{
			return false;
		}

		outValue = static_cast<int>(val);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void ProcessCommand(Car& car, const std::string& line)
{
	std::istringstream iss(line);
	std::string command;
	if (!(iss >> command))
	{
		return;
	}

	if (command == "Info")
	{
		PrintInfo(car);
	}
	else if (command == "EngineOn")
	{
		if (!car.TurnOnEngine())
		{
		}
	}
	else if (command == "EngineOff")
	{
		if (!car.TurnOffEngine())
		{
			std::cout << "Car must be stopped and in neutral gear\n";
		}
	}
	else if (command == "SetGear")
	{
		std::string argStr;
		if (!(iss >> argStr))
		{
			std::cout << "Invalid command argument\n";
			return;
		}

		int gear;
		if (!ParseIntArgument(argStr, gear))
		{
			std::cout << "Invalid command argument\n";
			return;
		}

		if (gear < -1 || gear > 5)
		{
			std::cout << "Invalid gear\n";
			return;
		}

		if (!car.SetGear(gear))
		{
			if (!car.IsTurnedOn())
			{
				std::cout << "Cannot set gear while engine is off\n";
			}
			else if (gear == -1 && car.GetSpeed() != 0)
			{
				std::cout << "Cannot reverse while moving\n";
			}
			else
			{
				std::cout << "Unsuitable current speed\n";
			}
		}
	}
	else if (command == "SetSpeed")
	{
		std::string argStr;
		if (!(iss >> argStr))
		{
			std::cout << "Invalid command argument\n";
			return;
		}

		int speed;
		if (!ParseIntArgument(argStr, speed))
		{
			std::cout << "Invalid command argument\n";
			return;
		}

		if (speed < 0)
		{
			std::cout << "Speed cannot be negative\n";
			return;
		}

		if (!car.SetSpeed(speed))
		{
			if (!car.IsTurnedOn())
			{
				std::cout << "Cannot set speed while engine is off\n";
			}
			else if (car.GetGear() == 0 && speed > car.GetSpeed())
			{
				std::cout << "Cannot accelerate on neutral\n";
			}
			else
			{
				std::cout << "Speed is out of gear range\n";
			}
		}
	}
	else
	{
		std::cout << "Unknown command\n";
	}
}

int main()
{

	try
	{
		Car car;
		std::string line;

		while (std::getline(std::cin, line))
		{
			ProcessCommand(car, line);
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}

	return 0;
}