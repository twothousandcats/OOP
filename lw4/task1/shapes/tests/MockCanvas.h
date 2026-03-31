#pragma once
#include "ICanvas.h"
#include "fakeit/fakeit.h"

using namespace fakeit;

struct MockCanvas : ICanvas
{
	void DrawLine(const CPoint& from, const CPoint& to, uint32_t lineColor) override
	{
	}

	void FillPolygon(const std::vector<CPoint>& points, uint32_t fillColor) override
	{
	}

	void DrawCircle(const CPoint& center, double radius, uint32_t lineColor) override
	{
	}

	void FillCircle(const CPoint& center, double radius, uint32_t fillColor) override
	{
	}
};