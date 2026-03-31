#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "fakeit/fakeit.h"

#include "CRectangle.h"
#include "CTriangle.h"
#include "CCircle.h"
#include "CLineSegment.h"

#include "MockCanvas.h"

using namespace Catch::Matchers;

TEST_CASE("Rectangle calculates area and perimeter correctly", "[geometry][rectangle]") {
    CPoint topLeft{10.0, 20.0};
    double width = 30.7;
    double height = 40.4;
    uint32_t outline = 0xFF0000;
    uint32_t fill = 0x00FF00;

    CRectangle rect(topLeft, width, height, outline, fill);

    SECTION("Area calculation") {
        double expectedArea = width * height;
        REQUIRE_THAT(rect.GetArea(), WithinAbs(expectedArea, 0.001));
    }

    SECTION("Perimeter calculation") {
        double expectedPerimeter = 2 * (width + height);
        REQUIRE_THAT(rect.GetPerimeter(), WithinAbs(expectedPerimeter, 0.001));
    }

    SECTION("Getters return correct values") {
        REQUIRE(rect.GetWidth() == width);
        REQUIRE(rect.GetHeight() == height);
        REQUIRE(rect.GetOutlineColor() == outline);
        REQUIRE(rect.GetFillColor() == fill);
    }
}

TEST_CASE("Triangle draws itself correctly on canvas", "[drawing][triangle]") {
	using namespace fakeit;

	Mock<MockCanvas> mockCanvas;
	std::vector<CPoint> capturedPoints;
    uint32_t capturedColor = 0;

	// Capture arguments when FillPolygon is called
	When(Method(mockCanvas, FillPolygon)).AlwaysReturn();
	When(Method(mockCanvas, FillPolygon)).AlwaysDo([&capturedPoints, &capturedColor](const std::vector<CPoint>& points, uint32_t color) {
		capturedPoints = points;
		capturedColor = color;
	});

	When(Method(mockCanvas, DrawLine)).AlwaysReturn();

	CPoint v1{0, 0}, v2{10, 0}, v3{5, 10};
	CTriangle triangle(v1, v2, v3, 0x0000FF, 0xFFFF00);

	triangle.Draw(mockCanvas.get());

	// Verify calls
	Verify(Method(mockCanvas, FillPolygon)).Once();
	Verify(Method(mockCanvas, DrawLine)).Exactly(3);

	REQUIRE(capturedPoints.size() == 3);
	REQUIRE(capturedPoints[0].x == v1.x);
	REQUIRE(capturedPoints[0].y == v1.y);
	REQUIRE(capturedPoints[1].x == v2.x);
	REQUIRE(capturedPoints[1].y == v2.y);
	REQUIRE(capturedPoints[2].x == v3.x);
	REQUIRE(capturedPoints[2].y == v3.y);
}

TEST_CASE("Circle calculates area and perimeter correctly", "[geometry][circle]") {
	constexpr CPoint center{0, 0};
	constexpr double radius = 5.0;
	constexpr double expectedArea = 3.14159265358979323846 * radius * radius;
	constexpr double expectedPerimeter = 2 * 3.14159265358979323846 * radius;

    const CCircle circle(center, radius, 0x000000, 0xFFFFFF);


    REQUIRE_THAT(circle.GetArea(), WithinAbs(expectedArea, 0.01));
    REQUIRE_THAT(circle.GetPerimeter(), WithinAbs(expectedPerimeter, 0.01));
}

TEST_CASE("LineSegment has zero area", "[geometry][line]") {
    CLineSegment line({0, 0}, {10, 10}, 0xFF0000);
    REQUIRE(line.GetArea() == 0.0);
    REQUIRE_THAT(line.GetPerimeter(), WithinAbs(std::hypot(10.0, 10.0), 0.001));
}