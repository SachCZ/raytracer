#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>
#include <physics.h> //TODO Includes physics just because of continue straight... resolve this
#include "../../support/matchers.h"


using namespace testing;
using namespace raytracer;

class IntersectionTest : public Test {
public:
    Point a{0, 0}, b{1, 0};
    Face face{0, {&a, &b}};
    MfemMesh mesh{SegmentedLine{0.0, 10.0, 5}, SegmentedLine{0.0, 10.0, 5}, mfem::Element::Type::TRIANGLE};
};

TEST_F(IntersectionTest, findClosestIntersection_returns_closest_intersection_if_provided_an_array_of_faces) {
    Ray ray{Point(0.5, -1), Vector(0, 1)};
    Point c{0, 1}, d{1, 1};
    Face anotherFace{0, {&c, &d}};
    auto pointOnFace = findClosestIntersectionPoint(ray, {&face, &anotherFace});
    ASSERT_THAT(pointOnFace->point, IsSamePoint(Point{0.5, 0}));
}

TEST_F(IntersectionTest, trace_through_steps_throught_mesh_according_to_find_intersection) {
    auto intersections = findIntersections(
            mesh,
            {Ray{Point(-1, 4.5), Vector(1, 0)}},
            {ContinueStraight{}},
            intersectStraight,
            dontStop
    );
    ASSERT_THAT(intersections[0], SizeIs(11));
}

TEST_F(IntersectionTest, intersecting_can_deal_with_diagonal_case) {
    Ray diagonalHalfLine{Point(-1, 9), Vector(1, -1)};

    auto intersections = findIntersections(
            mesh,
            {diagonalHalfLine},
            {ContinueStraight{}},
            intersectStraight,
            dontStop
    );
    ASSERT_THAT(intersections[0], SizeIs(19));
}

TEST_F(IntersectionTest, intersecting_can_be_done_using_multiple_functions) {
    auto intersections = findIntersections(
            mesh,
            {Ray{Point(-1, 9), Vector(1, 0)}},
            {
                    [](const PointOnFace &, const Vector &) {
                        return tl::optional<Vector>{};
                    },
                    [](const PointOnFace &, const Vector &) {
                        return Vector{1, -1};
                    },
                    ContinueStraight{}},
            intersectStraight,
            dontStop
    );
    ASSERT_THAT(intersections[0], SizeIs(19));
}