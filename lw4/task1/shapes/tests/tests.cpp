#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "fakeit/fakeit.h"

#include "CRectangle.h"
#include "CTriangle.h"
#include "CCircle.h"
#include "CLineSegment.h"
#include "MockCanvas.h"

using namespace Catch::Matchers;
using namespace fakeit;

TEST_CASE("Rectangle calculates area and perimeter correctly", "[geometry][rectangle]")
{
	CPoint topLeft{ 10.0, 20.0 };
	double width = 30.7;
	double height = 40.4;
	uint32_t outline = 0xFF0000;
	uint32_t fill = 0x00FF00;

	CRectangle rect(topLeft, width, height, outline, fill);

	SECTION("Area calculation")
	{
		double expectedArea = width * height;
		REQUIRE_THAT(rect.GetArea(), WithinAbs(expectedArea, 0.001));
	}

	SECTION("Perimeter calculation")
	{
		double expectedPerimeter = 2 * (width + height);
		REQUIRE_THAT(rect.GetPerimeter(), WithinAbs(expectedPerimeter, 0.001));
	}

	SECTION("Getters return correct values")
	{
		REQUIRE(rect.GetWidth() == width);
		REQUIRE(rect.GetHeight() == height);
		REQUIRE(rect.GetOutlineColor() == outline);
		REQUIRE(rect.GetFillColor() == fill);
	}
}

TEST_CASE("Rectangle Draw method calls canvas correctly", "[draw][rectangle]")
{
	MockCanvas mockCanvas;
	std::vector<CPoint> capturedPoints;
	uint32_t capturedFillColor = 0;
	std::vector<std::tuple<CPoint, CPoint, uint32_t> > capturedLines;

	When(Method(mockCanvas, FillPolygon)).AlwaysDo([&capturedPoints, &capturedFillColor](const std::vector<CPoint>& points, uint32_t color) {
		capturedPoints = points;
		capturedFillColor = color;
	});

	When(Method(mockCanvas, DrawLine)).AlwaysDo([&capturedLines](const CPoint& from, const CPoint& to, uint32_t color) {
		capturedLines.emplace_back(from, to, color);
	});

	CPoint leftTop{ 5.0, 10.0 };
	double width = 20.0;
	double height = 15.0;
	uint32_t outlineColor = 0xFF0000;
	uint32_t fillColor = 0x00FF00;

	CRectangle rect(leftTop, width, height, outlineColor, fillColor);
	rect.Draw(mockCanvas.get());

	Verify(Method(mockCanvas, FillPolygon)).Once();
	Verify(Method(mockCanvas, DrawLine)).Exactly(4);

	REQUIRE(capturedPoints.size() == 4);
	REQUIRE(capturedFillColor == fillColor);

	// Проверка точек полигона
	REQUIRE(capturedPoints[0].x == leftTop.x);
	REQUIRE(capturedPoints[0].y == leftTop.y);
	REQUIRE(capturedPoints[1].x == leftTop.x + width);
	REQUIRE(capturedPoints[1].y == leftTop.y);
	REQUIRE(capturedPoints[2].x == leftTop.x + width);
	REQUIRE(capturedPoints[2].y == leftTop.y + height);
	REQUIRE(capturedPoints[3].x == leftTop.x);
	REQUIRE(capturedPoints[3].y == leftTop.y + height);

	// Проверка линий
	REQUIRE(capturedLines.size() == 4);
	for (const auto& [from, to, color] : capturedLines)
	{
		REQUIRE(color == outlineColor);
	}
}

TEST_CASE("Triangle draws itself correctly on canvas", "[drawing][triangle]")
{
	MockCanvas mockCanvas;
	std::vector<CPoint> capturedPoints;
	uint32_t capturedColor = 0;
	std::vector<std::tuple<CPoint, CPoint, uint32_t> > capturedLines;

	When(Method(mockCanvas, FillPolygon)).AlwaysDo([&capturedPoints, &capturedColor](const std::vector<CPoint>& points, uint32_t color) {
		capturedPoints = points;
		capturedColor = color;
	});

	When(Method(mockCanvas, DrawLine)).AlwaysDo([&capturedLines](const CPoint& from, const CPoint& to, uint32_t color) {
		capturedLines.emplace_back(from, to, color);
	});

	CPoint v1{ 0, 0 }, v2{ 10, 0 }, v3{ 5, 10 };
	uint32_t outlineColor = 0x0000FF;
	uint32_t fillColor = 0xFFFF00;
	CTriangle triangle(v1, v2, v3, outlineColor, fillColor);

	triangle.Draw(mockCanvas.get());
	Verify(Method(mockCanvas, FillPolygon)).Once();
	Verify(Method(mockCanvas, DrawLine)).Exactly(3);

	REQUIRE(capturedPoints.size() == 3);
	REQUIRE(capturedColor == fillColor);
	REQUIRE(capturedPoints[0].x == v1.x);
	REQUIRE(capturedPoints[0].y == v1.y);
	REQUIRE(capturedPoints[1].x == v2.x);
	REQUIRE(capturedPoints[1].y == v2.y);
	REQUIRE(capturedPoints[2].x == v3.x);
	REQUIRE(capturedPoints[2].y == v3.y);

	REQUIRE(capturedLines.size() == 3);
	for (const auto& [from, to, color] : capturedLines)
	{
		REQUIRE(color == outlineColor);
	}
}

TEST_CASE("Circle draws itself correctly on canvas", "[draw][circle]")
{
	MockCanvas mockCanvas;
	CPoint capturedCenter;
	double capturedRadius = 0;
	uint32_t capturedFillColor = 0;
	uint32_t capturedOutlineColor = 0;
	bool fillCircleCalled = false;
	bool drawCircleCalled = false;

	When(Method(mockCanvas, FillCircle)).AlwaysDo([&capturedCenter, &capturedRadius, &capturedFillColor, &fillCircleCalled](const CPoint& center, double radius, uint32_t color) {
		capturedCenter = center;
		capturedRadius = radius;
		capturedFillColor = color;
		fillCircleCalled = true;
	});

	When(Method(mockCanvas, DrawCircle)).AlwaysDo([&capturedCenter, &capturedRadius, &capturedOutlineColor, &drawCircleCalled](const CPoint& center, double radius, uint32_t color) {
		capturedCenter = center;
		capturedRadius = radius;
		capturedOutlineColor = color;
		drawCircleCalled = true;
	});

	CPoint center{ 100.0, 200.0 };
	double radius = 50.0;
	uint32_t outlineColor = 0xFF0000;
	uint32_t fillColor = 0x00FF00;

	CCircle circle(center, radius, outlineColor, fillColor);
	circle.Draw(mockCanvas.get());

	Verify(Method(mockCanvas, FillCircle)).Once();
	Verify(Method(mockCanvas, DrawCircle)).Once();

	REQUIRE(fillCircleCalled);
	REQUIRE(drawCircleCalled);
	REQUIRE(capturedCenter.x == center.x);
	REQUIRE(capturedCenter.y == center.y);
	REQUIRE(capturedRadius == radius);
	REQUIRE(capturedFillColor == fillColor);
	REQUIRE(capturedOutlineColor == outlineColor);
}

TEST_CASE("Circle calculates area and perimeter correctly", "[geometry][circle]")
{
	constexpr CPoint center{ 0, 0 };
	constexpr double radius = 5.0;
	constexpr double expectedArea = 3.14159265358979323846 * radius * radius;
	constexpr double expectedPerimeter = 2 * 3.14159265358979323846 * radius;

	const CCircle circle(center, radius, 0x000000, 0xFFFFFF);

	REQUIRE_THAT(circle.GetArea(), WithinAbs(expectedArea, 0.01));
	REQUIRE_THAT(circle.GetPerimeter(), WithinAbs(expectedPerimeter, 0.01));
}

TEST_CASE("LineSegment has zero area", "[geometry][line]")
{
	CLineSegment line({ 0, 0 }, { 10, 10 }, 0xFF0000);
	REQUIRE(line.GetArea() == 0.0);
	REQUIRE_THAT(line.GetPerimeter(), WithinAbs(std::hypot(10.0, 10.0), 0.001));
}