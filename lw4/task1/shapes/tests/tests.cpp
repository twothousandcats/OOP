#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "fakeit/fakeit.h"

#include "CRectangle.h"
#include "CTriangle.h"
#include "CCircle.h"
#include "CLineSegment.h"
#include "MockCanvas.h"
#include <cmath> // for std::hypot if not included elsewhere

using namespace Catch::Matchers;
using namespace fakeit;

// Helper matcher for points to keep tests concise
static bool PointsEqual(const CPoint& a, const CPoint& b, double epsilon = 0.0001) {
    return std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon;
}

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
	std::vector<std::tuple<CPoint, CPoint, uint32_t>> capturedLines;

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

	// Define expected corners
	CPoint p1 = leftTop; // Top-Left
	CPoint p2{ leftTop.x + width, leftTop.y }; // Top-Right
	CPoint p3{ leftTop.x + width, leftTop.y + height }; // Bottom-Right
	CPoint p4{ leftTop.x, leftTop.y + height }; // Bottom-Left

	// Check polygon points order (assuming clockwise or counter-clockwise consistency)
	REQUIRE(PointsEqual(capturedPoints[0], p1));
	REQUIRE(PointsEqual(capturedPoints[1], p2));
	REQUIRE(PointsEqual(capturedPoints[2], p3));
	REQUIRE(PointsEqual(capturedPoints[3], p4));

	// Check lines coordinates and colors
	REQUIRE(capturedLines.size() == 4);

	// Expected lines: P1->P2, P2->P3, P3->P4, P4->P1
	// Note: Adjust order if your implementation draws differently, but it must be consistent
	REQUIRE(PointsEqual(std::get<0>(capturedLines[0]), p1));
	REQUIRE(PointsEqual(std::get<1>(capturedLines[0]), p2));
	REQUIRE(std::get<2>(capturedLines[0]) == outlineColor);

	REQUIRE(PointsEqual(std::get<0>(capturedLines[1]), p2));
	REQUIRE(PointsEqual(std::get<1>(capturedLines[1]), p3));
	REQUIRE(std::get<2>(capturedLines[1]) == outlineColor);

	REQUIRE(PointsEqual(std::get<0>(capturedLines[2]), p3));
	REQUIRE(PointsEqual(std::get<1>(capturedLines[2]), p4));
	REQUIRE(std::get<2>(capturedLines[2]) == outlineColor);

	REQUIRE(PointsEqual(std::get<0>(capturedLines[3]), p4));
	REQUIRE(PointsEqual(std::get<1>(capturedLines[3]), p1));
	REQUIRE(std::get<2>(capturedLines[3]) == outlineColor);
}

TEST_CASE("Triangle draws itself correctly on canvas", "[drawing][triangle]")
{
	MockCanvas mockCanvas;
	std::vector<CPoint> capturedPoints;
	uint32_t capturedColor = 0;
	std::vector<std::tuple<CPoint, CPoint, uint32_t>> capturedLines;

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

	// Check vertices
	REQUIRE(PointsEqual(capturedPoints[0], v1));
	REQUIRE(PointsEqual(capturedPoints[1], v2));
	REQUIRE(PointsEqual(capturedPoints[2], v3));

	REQUIRE(capturedLines.size() == 3);

	// Check lines: V1->V2, V2->V3, V3->V1
	REQUIRE(PointsEqual(std::get<0>(capturedLines[0]), v1));
	REQUIRE(PointsEqual(std::get<1>(capturedLines[0]), v2));
	REQUIRE(std::get<2>(capturedLines[0]) == outlineColor);

	REQUIRE(PointsEqual(std::get<0>(capturedLines[1]), v2));
	REQUIRE(PointsEqual(std::get<1>(capturedLines[1]), v3));
	REQUIRE(std::get<2>(capturedLines[1]) == outlineColor);

	REQUIRE(PointsEqual(std::get<0>(capturedLines[2]), v3));
	REQUIRE(PointsEqual(std::get<1>(capturedLines[2]), v1));
	REQUIRE(std::get<2>(capturedLines[2]) == outlineColor);
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

	REQUIRE(PointsEqual(capturedCenter, center));
	REQUIRE_THAT(capturedRadius, WithinAbs(radius, 0.0001));
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

TEST_CASE("LineSegment has zero area and correct length", "[geometry][line]")
{
	CPoint start{ 0, 0 };
	CPoint end{ 10, 10 };
	uint32_t color = 0xFF0000;

	CLineSegment line(start, end, color);

	REQUIRE(line.GetArea() == 0.0);

	double expectedLength = std::hypot(end.x - start.x, end.y - start.y);
	REQUIRE_THAT(line.GetPerimeter(), WithinAbs(expectedLength, 0.001));
}

TEST_CASE("LineSegment draws itself correctly", "[draw][line]")
{
    MockCanvas mockCanvas;
    std::vector<std::tuple<CPoint, CPoint, uint32_t>> capturedLines;

    When(Method(mockCanvas, DrawLine)).AlwaysDo([&capturedLines](const CPoint& from, const CPoint& to, uint32_t color) {
        capturedLines.emplace_back(from, to, color);
    });

    CPoint start{ 1.5, 2.5 };
    CPoint end{ 11.5, 12.5 };
    uint32_t color = 0x123456;

    CLineSegment line(start, end, color);
    line.Draw(mockCanvas.get());

    Verify(Method(mockCanvas, DrawLine)).Once();
    REQUIRE(capturedLines.size() == 1);

    auto [from, to, drawnColor] = capturedLines[0];
    REQUIRE(PointsEqual(from, start));
    REQUIRE(PointsEqual(to, end));
    REQUIRE(drawnColor == color);
}