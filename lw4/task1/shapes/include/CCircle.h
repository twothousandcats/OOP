#pragma once
#include "ICanvasDrawable.h"
#include "CPoint.h"
#include "CSolidShape.h"

class CCircle : public CSolidShape, public ICanvasDrawable
{
public:
	explicit CCircle(CPoint center, double radius, uint32_t outlineColor, uint32_t fillColor);

	double GetArea() const override;

	double GetPerimeter() const override;

	std::string ToString() const override;

	void Draw(ICanvas& canvas) const override;

	CPoint GetCenter() const;

	double GetRadius() const;

private:
	CPoint m_center;
	double m_radius;
};