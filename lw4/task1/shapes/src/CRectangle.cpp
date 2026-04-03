#include "CRectangle.h"
#include "ICanvas.h"
#include <sstream>
#include <iomanip>
#include <vector>

CRectangle::CRectangle(CPoint leftTop, double width, double height, uint32_t outlineColor, uint32_t fillColor)
	: CSolidShape(outlineColor, fillColor), m_leftTop(leftTop), m_width(width), m_height(height)
{
}

double CRectangle::GetArea() const
{
	return m_width * m_height;
}

double CRectangle::GetPerimeter() const
{
	return 2 * (m_width + m_height);
}

std::string CRectangle::ToString() const
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2);
	oss << "Rectangle with top-left at (" << m_leftTop.x << ", " << m_leftTop.y
		<< "), width " << m_width << ", height " << m_height;
	return oss.str();
}

CPoint CRectangle::GetLeftTop() const { return m_leftTop; }
CPoint CRectangle::GetRightBottom() const { return { m_leftTop.x + m_width, m_leftTop.y + m_height }; }
double CRectangle::GetWidth() const { return m_width; }
double CRectangle::GetHeight() const { return m_height; }

void CRectangle::Draw(ICanvas& canvas) const
{
	std::vector points = {
		m_leftTop,
		{ m_leftTop.x + m_width, m_leftTop.y },
		{ m_leftTop.x + m_width, m_leftTop.y + m_height },
		{ m_leftTop.x, m_leftTop.y + m_height }
	};
	canvas.FillPolygon(points, m_fillColor);
	canvas.DrawLine(points[0], points[1], m_outlineColor);
	canvas.DrawLine(points[1], points[2], m_outlineColor);
	canvas.DrawLine(points[2], points[3], m_outlineColor);
	canvas.DrawLine(points[3], points[0], m_outlineColor);
}