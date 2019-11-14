#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <raytracer/geometry/Point.h>
#include <raytracer/geometry/Quadrilateral.h>

using namespace testing;

class initialized_quad : public Test {
public:
    using Point = raytracer::geometry::Point;
    using Quad = raytracer::geometry::Quadrilateral;

    std::vector<Point> points{Point(0, 0), Point(1, 0), Point(1, 1), Point(0, 1)};
    Quad quad{points};
};

TEST_F(initialized_quad, has_consists_of_three_specific_points) {
    auto trianglePoints = quad.getPoints();
    EXPECT_THAT(trianglePoints, SizeIs(4));
    ASSERT_THAT(trianglePoints[1].x, DoubleEq(1)); //One random test
}

TEST_F(initialized_quad, has_three_specific_edges) {
    auto& edges = quad.edges;
    EXPECT_THAT(edges, SizeIs(4));

    EXPECT_THAT(edges[0].startPoint.x, DoubleEq(0));
    EXPECT_THAT(edges[0].startPoint.y, DoubleEq(0));
    EXPECT_THAT(edges[0].endPoint.x, DoubleEq(1));
    EXPECT_THAT(edges[0].endPoint.y, DoubleEq(0));

    EXPECT_THAT(edges[1].startPoint.x, DoubleEq(1));
    EXPECT_THAT(edges[1].startPoint.y, DoubleEq(0));
    EXPECT_THAT(edges[1].endPoint.x, DoubleEq(1));
    EXPECT_THAT(edges[1].endPoint.y, DoubleEq(1));

    EXPECT_THAT(edges[2].startPoint.x, DoubleEq(1));
    EXPECT_THAT(edges[2].startPoint.y, DoubleEq(1));
    EXPECT_THAT(edges[2].endPoint.x, DoubleEq(0));
    EXPECT_THAT(edges[2].endPoint.y, DoubleEq(1));
}



