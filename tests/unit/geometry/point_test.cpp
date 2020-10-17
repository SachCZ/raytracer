#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../matchers.h"
#include <geometry.h>

using namespace testing;
using Point = raytracer::Point;
using Vector = raytracer::Vector;

class PointTest : public Test {
public:
    Point A{2, 5};
    Point B{4, -2.5};
    Vector b{8, -5};

};

TEST_F(PointTest, point_has_x) {
    ASSERT_THAT(A.x, DoubleEq(2));
}

TEST_F(PointTest, point_has_y) {
    ASSERT_THAT(A.y, DoubleEq(5));
}

TEST_F(PointTest, points_support_subtraction){
    Vector result = A - B;
    ASSERT_THAT(result, IsSameVector(Vector(-2, 7.5)));
}

TEST_F(PointTest, could_be_added){
    Point result = A + b;
    ASSERT_THAT(result, IsSamePoint(Point(10, 0)));
}

TEST_F(PointTest, addition_order_does_not_matter){
    Point result = b + A;
    ASSERT_THAT(result, IsSamePoint(Point(10, 0)));
}