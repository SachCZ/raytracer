#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../matchers.h"
#include "raytracer/geometry/Intersection.h"

using namespace testing;
using namespace raytracer;

class find_intersection : public Test {
public:
    Point a{0, 0}, b{1, 0};
    Face face{0, {&a, &b}};
};

TEST_F(find_intersection, returns_the_intersection_if_line_and_face_are_intersecting) {
    HalfLine halfLine{Point(0.5, -1), Vector(0, 1)};
    auto pointOnFace = findIntersection(halfLine, &face);
    ASSERT_THAT(pointOnFace->point, IsSamePoint(Point{0.5, 0}));
}

TEST_F(find_intersection, returns_null_if_line_and_edge_are_not_intersecting) {
    HalfLine halfLine{Point(0.5, -1), Vector(2, 1)};
    auto intersection = findIntersection(halfLine, &face);
    EXPECT_THAT(intersection, IsNull());
}

TEST_F(find_intersection, returns_intersection_if_border_point_is_intersected) {
    HalfLine halfLine{Point(0.5, -0.5), Vector(1, 1)};
    auto pointOnFace = findIntersection(halfLine, &face);
    ASSERT_THAT(pointOnFace->point, IsSamePoint(Point{1, 0}));
}

TEST_F(find_intersection, closeset_returns_closest_intersection_if_provided_an_array_of_faces) {
    HalfLine halfLine{Point(0.5, -1), Vector(0, 1)};
    Point c{0, 1}, d{1, 1};
    Face anotherFace{0, {&c, &d}};
    auto pointOnFace = findClosestIntersection(halfLine, {&face, &anotherFace});
    ASSERT_THAT(pointOnFace->point, IsSamePoint(Point{0.5, 0}));
}