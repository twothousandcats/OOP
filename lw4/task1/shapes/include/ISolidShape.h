#pragma once
#include "IShape.h"

// todo: вынести реализацию fillColor в одно место
// todo: вынести реализацию outlineColor в одно место
class ISolidShape : public IShape
{
public:
	virtual uint32_t GetFillColor() const = 0;
};