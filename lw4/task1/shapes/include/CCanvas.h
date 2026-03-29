#pragma once
#include "ICanvas.h"
#include <SFML/Graphics.hpp>

class CCanvas : public ICanvas {
public:
	explicit CCanvas(sf::RenderWindow& window);

	void DrawLine(const CPoint& from, const CPoint& to, uint32_t lineColor) override;
	void FillPolygon(const std::vector<CPoint>& points, uint32_t fillColor) override;
	void DrawCircle(const CPoint& center, double radius, uint32_t lineColor) override;
	void FillCircle(const CPoint& center, double radius, uint32_t fillColor) override;

private:
	sf::RenderWindow& m_window;

	static sf::Color ToSfColor(uint32_t color); // convert
};