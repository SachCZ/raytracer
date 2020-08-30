#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../matchers.h"
#include <raytracer/geometry/Point.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using Point = raytracer::Point;
using Vector = raytracer::Vector;

class initialized_point : public Test {
public:
    Point point{2, 5};
};

TEST_F(initialized_point, has_x) {
    ASSERT_THAT(point.x, DoubleEq(2));
}

TEST_F(initialized_point, has_y) {
    ASSERT_THAT(point.y, DoubleEq(5));
}

class two_points : public Test {
public:
    Point A{3, -2};
    Point B{4, -2.5};
};

TEST_F(two_points, support_subtraction){
    Vector result = A - B;
    ASSERT_THAT(result, IsSameVector(Vector(-1, 0.5)));
}

class vector_and_point : public Test {
public:
    Point A{2, 1.2};
    Vector b{8, -5};
};

TEST_F(vector_and_point, could_be_added){
    Point result = A + b;
    ASSERT_THAT(result, IsSamePoint(Point(10, -3.8)));
}

TEST_F(vector_and_point, addition_order_does_not_matter){
    Point result = b + A;
    ASSERT_THAT(result, IsSamePoint(Point(10, -3.8)));
}