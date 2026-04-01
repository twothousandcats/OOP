#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "CPoint.h"
#include "IShape.h"
#include "ISolidShape.h"
#include "ICanvasDrawable.h"
#include "CCanvas.h"
#include "CLineSegment.h"
#include "CTriangle.h"
#include "CRectangle.h"
#include "CCircle.h"
#include <SFML/Graphics.hpp>

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;
constexpr sf::Vector2u WINDOW_SIZE = { WINDOW_WIDTH, WINDOW_HEIGHT };
constexpr std::string WINDOW_TITLE = "Shapes";

std::unique_ptr<IShape> ParseShape(const std::string& line)
{
	std::istringstream iss(line);
	std::string type;
	iss >> type;

	if (type == "rectangle")
	{
		double x, y, width, height;
		uint32_t outlineColor, fillColor;
		iss >> x >> y >> width >> height >> std::hex >> outlineColor >> std::hex >> fillColor;
		return std::make_unique<CRectangle>(CPoint{ x, y }, width, height, outlineColor, fillColor);
	}
	if (type == "triangle")
	{
		double x1, y1, x2, y2, x3, y3;
		uint32_t outlineColor, fillColor;
		iss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> std::hex >> outlineColor >> std::hex >> fillColor;
		return std::make_unique<CTriangle>(CPoint{ x1, y1 }, CPoint{ x2, y2 }, CPoint{ x3, y3 }, outlineColor, fillColor);
	}
	if (type == "circle")
	{
		double x, y, radius;
		uint32_t outlineColor, fillColor;
		iss >> x >> y >> radius >> std::hex >> outlineColor >> std::hex >> fillColor;
		return std::make_unique<CCircle>(CPoint{ x, y }, radius, outlineColor, fillColor);
	}
	if (type == "line")
	{
		double x1, y1, x2, y2;
		uint32_t outlineColor;
		iss >> x1 >> y1 >> x2 >> y2 >> std::hex >> outlineColor;
		return std::make_unique<CLineSegment>(CPoint{ x1, y1 }, CPoint{ x2, y2 }, outlineColor);
	}

	return nullptr;
}

int main()
{
	// auto remove from heap
	// защита от копирования
	// Нельзя создать вектор объектов абстрактного класса
	// shared_ptr допускает совместное владение
	std::vector<std::unique_ptr<IShape> > shapes;
	std::string line;

	while (std::getline(std::cin, line))
	{
		if (auto shape = ParseShape(line))
		{
			// unique_ptr
			// cant copied, cant be moved push_back by value
			// so std::move
			shapes.push_back(std::move(shape));
		}
	}

	if (shapes.empty())
	{
		std::cout << "No shapes provided." << std::endl;
		return 0;
	}

	const auto maxAreaShape = std::ranges::max_element(shapes,
		[](const auto& a, const auto& b) { return a->GetArea() < b->GetArea(); });

	auto minPerimeterShape = std::ranges::min_element(shapes,
		[](const auto& a, const auto& b) { return a->GetPerimeter() < b->GetPerimeter(); });

	std::cout << std::fixed << std::setprecision(2);
	std::cout << "Shape with largest area:\n";
	std::cout << "Area: " << (*maxAreaShape)->GetArea() << "\n";
	std::cout << "Perimeter: " << (*maxAreaShape)->GetPerimeter() << "\n";
	std::cout << "Outline Color: #" << std::hex << (*maxAreaShape)->GetOutlineColor() << "\n";
	if (const auto solid = dynamic_cast<ISolidShape*>(maxAreaShape->get()))
	{
		std::cout << "Fill Color: #" << std::hex << solid->GetFillColor() << "\n";
	}
	std::cout << (*maxAreaShape)->ToString() << "\n\n";

	std::cout << "Shape with smallest perimeter:\n";
	std::cout << "Area: " << (*minPerimeterShape)->GetArea() << "\n";
	std::cout << "Perimeter: " << (*minPerimeterShape)->GetPerimeter() << "\n";
	std::cout << "Outline Color: #" << std::hex << (*minPerimeterShape)->GetOutlineColor() << "\n";
	if (auto solid = dynamic_cast<ISolidShape*>(minPerimeterShape->get()))
	{
		std::cout << "Fill Color: #" << std::hex << solid->GetFillColor() << "\n";
	}
	std::cout << (*minPerimeterShape)->ToString() << "\n";

	sf::RenderWindow window(
		sf::VideoMode(WINDOW_SIZE),
		WINDOW_TITLE
		);
	CCanvas canvas(window);

	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}

		window.clear(sf::Color::White);
		for (const auto& shape : shapes)
		{
			if (const auto* drawable = dynamic_cast<const ICanvasDrawable*>(shape.get()))
			{
				drawable->Draw(canvas);
			}
		}

		window.display();
	}

	return 0;
}