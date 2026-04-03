#pragma once
#include "ISolidShape.h"
#include "ICanvasDrawable.h"
#include "CPoint.h"

class CTriangle : public ISolidShape, public ICanvasDrawable
{
public:
	explicit CTriangle(CPoint v1, CPoint v2, CPoint v3, uint32_t outlineColor, uint32_t fillColor);

	double GetArea() const override;

	double GetPerimeter() const override;

	std::string ToString() const override;

	uint32_t GetOutlineColor() const override;

	uint32_t GetFillColor() const override;

	void Draw(ICanvas& canvas) const override;

	CPoint GetVertex1() const;

	CPoint GetVertex2() const;

	CPoint GetVertex3() const;

private:
	CPoint m_v1, m_v2, m_v3;
	uint32_t m_outlineColor;
	uint32_t m_fillColor;
};