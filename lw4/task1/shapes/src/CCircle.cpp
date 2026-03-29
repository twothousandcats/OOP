#include "CCircle.h"
#include "ICanvas.h"
#include <sstream>
#include <iomanip>

CCircle::CCircle(CPoint center, double radius, uint32_t outlineColor, uint32_t fillColor)
	: m_center(center), m_radius(radius), m_outlineColor(outlineColor), m_fillColor(fillColor) {}

double CCircle::GetArea() const {
	return M_PI * m_radius * m_radius;
}

double CCircle::GetPerimeter() const {
	return 2 * M_PI * m_radius;
}

std::string CCircle::ToString() const {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2);
	oss << "Circle with center at (" << m_center.x << ", " << m_center.y
		<< ") and radius " << m_radius;
	return oss.str();
}

uint32_t CCircle::GetOutlineColor() const { return m_outlineColor; }
uint32_t CCircle::GetFillColor() const { return m_fillColor; }
CPoint CCircle::GetCenter() const { return m_center; }
double CCircle::GetRadius() const { return m_radius; }

void CCircle::Draw(ICanvas& canvas) const {
	canvas.FillCircle(m_center, m_radius, m_fillColor);
	canvas.DrawCircle(m_center, m_radius, m_outlineColor);
}