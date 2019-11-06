#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>

using namespace testing;

class initialized_edge : public Test {
public:
    using Point = raytracer::geometry::Point;
    using Edge = raytracer::geometry::Edge;

    Point startPoint{2, 5};
    Point endPoint{5, 7};

    Edge edge{startPoint, endPoint};

};

TEST_F(initialized_edge, has_proper_start_point) {
    EXPECT_THAT(edge.startPoint.x, DoubleEq(2));
    ASSERT_THAT(edge.startPoint.y, DoubleEq(5));
}

TEST_F(initialized_edge, has_proper_end_point) {
    EXPECT_THAT(edge.endPoint.x, DoubleEq(5));
    ASSERT_THAT(edge.endPoint.y, DoubleEq(7));
}

