#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>
#include <physics.h> //TODO Includes physics just because of continue straight... resolve this
#include "../matchers.h"


using namespace testing;
using namespace raytracer;

class IntersectionTest : public Test {
public:
    Point a{0, 0}, b{1, 0};
    Face face{0, {&a, &b}};
    MfemMesh mesh{DiscreteLine{10.0, 5}, DiscreteLine{10.0, 5}};
};

TEST_F(IntersectionTest, findIntersection_returns_the_intersection_if_line_and_face_are_intersecting) {
    HalfLine halfLine{Point(0.5, -1), Vector(0, 1)};
    auto pointOnFace = findIntersectionPoint(halfLine, &face);
    ASSERT_THAT(pointOnFace->point, IsSamePoint(Point{0.5, 0}));
}

TEST_F(IntersectionTest, findIntersection_returns_null_if_line_and_edge_are_not_intersecting) {
    HalfLine halfLine{Point(0.5, -1), Vector(2, 1)};
    auto intersection = findIntersectionPoint(halfLine, &face);
    ASSERT_THAT(intersection, IsNull());
}

TEST_F(IntersectionTest, findIntersection_returns_intersection_if_border_point_is_intersected) {
    HalfLine halfLine{Point(0.5, -0.5), Vector(1, 1)};
    auto pointOnFace = findIntersectionPoint(halfLine, &face);
    ASSERT_THAT(pointOnFace->point, IsSamePoint(Point{1, 0}));
}

TEST_F(IntersectionTest, findClosestIntersection_returns_closest_intersection_if_provided_an_array_of_faces) {
    HalfLine halfLine{Point(0.5, -1), Vector(0, 1)};
    Point c{0, 1}, d{1, 1};
    Face anotherFace{0, {&c, &d}};
    auto pointOnFace = findClosestIntersectionPoint(halfLine, {&face, &anotherFace});
    ASSERT_THAT(pointOnFace->point, IsSamePoint(Point{0.5, 0}));
}

TEST_F(IntersectionTest, trace_through_steps_throught_mesh_according_to_find_intersection) {
    auto intersections = findIntersections(
            mesh,
            HalfLine{Point(-1, 4.5), Vector(1, 0)},
            ContinueStraight{},
            intersectStraight,
            DontStop{}
    );
    ASSERT_THAT(intersections, SizeIs(11));
}

TEST_F(IntersectionTest, intersecting_can_deal_with_diagonal_case) {
    HalfLine diagonalHalfLine{Point(-1, 9), Vector(1, -1)};

    auto intersections = findIntersections(
            mesh,
            diagonalHalfLine,
            ContinueStraight{},
            intersectStraight,
            DontStop{}
    );
    ASSERT_THAT(intersections, SizeIs(19));
}