#include "CTriangle.h"
#include "ICanvas.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <vector>

CTriangle::CTriangle(CPoint v1, CPoint v2, CPoint v3, uint32_t outlineColor, uint32_t fillColor)
	: m_v1(v1), m_v2(v2), m_v3(v3), m_outlineColor(outlineColor), m_fillColor(fillColor) {}

double CTriangle::GetArea() const {
	return std::abs((m_v1.x * (m_v2.y - m_v3.y) +
					 m_v2.x * (m_v3.y - m_v1.y) +
					 m_v3.x * (m_v1.y - m_v2.y)) / 2.0);
}

double CTriangle::GetPerimeter() const {
	return std::hypot(m_v2.x - m_v1.x, m_v2.y - m_v1.y) +
		   std::hypot(m_v3.x - m_v2.x, m_v3.y - m_v2.y) +
		   std::hypot(m_v1.x - m_v3.x, m_v1.y - m_v3.y);
}

std::string CTriangle::ToString() const {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2);
	oss << "Triangle with vertices at (" << m_v1.x << ", " << m_v1.y << "), ("
		<< m_v2.x << ", " << m_v2.y << "), (" << m_v3.x << ", " << m_v3.y << ")";
	return oss.str();
}

uint32_t CTriangle::GetOutlineColor() const { return m_outlineColor; }
uint32_t CTriangle::GetFillColor() const { return m_fillColor; }
CPoint CTriangle::GetVertex1() const { return m_v1; }
CPoint CTriangle::GetVertex2() const { return m_v2; }
CPoint CTriangle::GetVertex3() const { return m_v3; }

void CTriangle::Draw(ICanvas& canvas) const {
	std::vector<CPoint> points = {m_v1, m_v2, m_v3};
	canvas.FillPolygon(points, m_fillColor);
	canvas.DrawLine(m_v1, m_v2, m_outlineColor);
	canvas.DrawLine(m_v2, m_v3, m_outlineColor);
	canvas.DrawLine(m_v3, m_v1, m_outlineColor);
}