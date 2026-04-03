#include "CCanvas.h"
#include <SFML/Graphics.hpp>

CCanvas::CCanvas(sf::RenderWindow& window)
	: m_window(window)
{
}

sf::Color CCanvas::ToSfColor(uint32_t color)
{
	// 0xRRGGBB
	uint8_t a = (color >> 24) & 0xFF;
	uint8_t r = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = color & 0xFF;

	if (a == 0 && color != 0)
	{
		// set default alpha
		a = 255;
	}

	return { r, g, b, a };
}

void CCanvas::DrawLine(const CPoint& from, const CPoint& to, const uint32_t lineColor)
{
	const sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(static_cast<float>(from.x), static_cast<float>(from.y)), ToSfColor(lineColor)),
		sf::Vertex(sf::Vector2f(static_cast<float>(to.x), static_cast<float>(to.y)), ToSfColor(lineColor))
	};
	m_window.draw(line, 2, sf::PrimitiveType::Lines);
}

void CCanvas::FillPolygon(const std::vector<CPoint>& points, const uint32_t fillColor)
{
	if (points.size() < 3)
	{
		return;
	}

	// todo: range-base for - done
	sf::ConvexShape polygon(points.size());
	for (int i = 0; const auto [x, y] : points)
	{
		polygon.setPoint(i, sf::Vector2f(static_cast<float>(x), static_cast<float>(y)));
		i++;
	}
	polygon.setFillColor(ToSfColor(fillColor));
	polygon.setOutlineColor(sf::Color::Transparent);
	m_window.draw(polygon);
}

void CCanvas::DrawCircle(const CPoint& center, const double radius, const uint32_t lineColor)
{
	if (radius <= 0)
		return;
	sf::CircleShape circle(static_cast<float>(radius));
	circle.setPosition({ static_cast<float>(center.x), static_cast<float>(center.y) });
	circle.setOrigin({ static_cast<float>(radius), static_cast<float>(radius) }); // center
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineThickness(1.0f);
	circle.setOutlineColor(ToSfColor(lineColor));
	m_window.draw(circle);
}

void CCanvas::FillCircle(const CPoint& center, const double radius, const uint32_t fillColor)
{
	if (radius <= 0)
		return;
	sf::CircleShape circle(static_cast<float>(radius));
	circle.setPosition({ static_cast<float>(center.x), static_cast<float>(center.y) });
	circle.setOrigin({ static_cast<float>(radius), static_cast<float>(radius) }); // center
	circle.setFillColor(ToSfColor(fillColor));
	circle.setOutlineColor(sf::Color::Transparent);
	m_window.draw(circle);
}