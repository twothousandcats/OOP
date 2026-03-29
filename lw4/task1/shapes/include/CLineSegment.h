#pragma once
#include "IShape.h"
#include "CPoint.h"

class CLineSegment : public IShape
{
public:
	CLineSegment(CPoint start, CPoint end, uint32_t outlineColor);

	double GetArea() const override;

	double GetPerimeter() const override;

	std::string ToString() const override;

	uint32_t GetOutlineColor() const override;

	CPoint GetStartPoint() const;

	CPoint GetEndPoint() const;

private:
	CPoint m_start;
	CPoint m_end;
	uint32_t m_outlineColor;
};