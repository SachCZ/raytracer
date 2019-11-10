#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <geometry/Point.h>
#include <geometry/Triangle.h>

using namespace testing;

class initialized_triangle : public Test {
public:
    using Point = raytracer::geometry::Point;
    using Traingle = raytracer::geometry::Triangle;

    std::vector<Point> points{Point(0, 0), Point(2, 0), Point(1, 1)};
    Traingle triangle{points};
};

TEST_F(initialized_triangle, has_consists_of_three_specific_points) {
    auto trianglePoints = triangle.getPoints();
    EXPECT_THAT(trianglePoints, SizeIs(3));
    ASSERT_THAT(trianglePoints[1].x, DoubleEq(2)); //One random test
}

TEST_F(initialized_triangle, has_three_specific_edges) {
    auto& edges = triangle.edges;
    EXPECT_THAT(edges, SizeIs(3));

    EXPECT_THAT(edges[0].startPoint.x, DoubleEq(0));
    EXPECT_THAT(edges[0].startPoint.y, DoubleEq(0));
    EXPECT_THAT(edges[0].endPoint.x, DoubleEq(2));
    EXPECT_THAT(edges[0].endPoint.y, DoubleEq(0));

    EXPECT_THAT(edges[1].startPoint.x, DoubleEq(2));
    EXPECT_THAT(edges[1].startPoint.y, DoubleEq(0));
    EXPECT_THAT(edges[1].endPoint.x, DoubleEq(1));
    EXPECT_THAT(edges[1].endPoint.y, DoubleEq(1));

    EXPECT_THAT(edges[2].startPoint.x, DoubleEq(1));
    EXPECT_THAT(edges[2].startPoint.y, DoubleEq(1));
    EXPECT_THAT(edges[2].endPoint.x, DoubleEq(0));
    EXPECT_THAT(edges[2].endPoint.x, DoubleEq(0));
    EXPECT_THAT(edges[2].endPoint.y, DoubleEq(0));
}



