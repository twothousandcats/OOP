#include "CLineSegment.h"

#include "ICanvas.h"

#include <cmath>
#include <sstream>
#include <iomanip>

CLineSegment::CLineSegment(CPoint start, CPoint end, uint32_t outlineColor)
	: m_start(start), m_end(end), m_outlineColor(outlineColor)
{
}

double CLineSegment::GetArea() const
{
	return 0.0;
}

double CLineSegment::GetPerimeter() const
{
	return std::hypot(m_end.x - m_start.x, m_end.y - m_start.y);
}

std::string CLineSegment::ToString() const
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2);
	oss << "Line Segment from (" << m_start.x << ", " << m_start.y
		<< ") to (" << m_end.x << ", " << m_end.y << ")";
	return oss.str();
}

uint32_t CLineSegment::GetOutlineColor() const
{
	return m_outlineColor;
}

CPoint CLineSegment::GetStartPoint() const
{
	return m_start;
}

CPoint CLineSegment::GetEndPoint() const
{
	return m_end;
}

void CLineSegment::Draw(ICanvas& canvas) const
{
	canvas.DrawLine(m_start, m_end, m_outlineColor);
}