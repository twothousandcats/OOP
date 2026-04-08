#pragma once
#include "IShape.h"

class CShape : public IShape
{
public:
	explicit CShape(uint32_t outlineColor);

	uint32_t GetOutlineColor() const override = 0;

private:
	uint32_t m_outlineColor;
};