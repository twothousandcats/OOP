#pragma once
#include "ICanvasDrawable.h"
#include "CPoint.h"
#include "CSolidShape.h"

class CRectangle : public CSolidShape, public ICanvasDrawable
{
public:
	explicit CRectangle(CPoint leftTop, double width, double height, uint32_t outlineColor, uint32_t fillColor);

	double GetArea() const override;

	double GetPerimeter() const override;

	std::string ToString() const override;

	void Draw(ICanvas& canvas) const override;

	CPoint GetLeftTop() const;

	CPoint GetRightBottom() const;

	double GetWidth() const;

	double GetHeight() const;

private:
	CPoint m_leftTop;
	double m_width;
	double m_height;
};