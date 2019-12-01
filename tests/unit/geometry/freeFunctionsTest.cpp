#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "raytracer/geometry/FreeFunctions.h"
#include "raytracer/geometry/Point.h"
#include "raytracer/geometry/Vector.h"
#include "raytracer/geometry/Mesh.h"

using namespace testing;
using namespace raytracer::geometry;

class find_intersection : public Test {
public:
    DiscreteLine sideA{1 , 1};
    Mesh mesh{sideA, sideA};
};

TEST_F(find_intersection, returns_the_intersection_if_line_and_edge_are_intersecting) {
    HalfLine halfLine{Point(0.5, -1), Vector(0, 1)};
    auto face = mesh.getBoundary()[0];
    auto intersection = findIntersection(halfLine, face);
    EXPECT_THAT(intersection->halfLine.point.x, DoubleEq(0.5));
    ASSERT_THAT(intersection->halfLine.point.y, DoubleEq(0));
}

TEST_F(find_intersection, returns_null_if_line_and_edge_are_not_intersecting) {
    HalfLine halfLine{Point(0.5, -1), Vector(2, 1)};
    auto face = mesh.getBoundary()[0];
    auto intersection = findIntersection(halfLine, face);
    EXPECT_THAT(intersection, IsNull());
}

TEST_F(find_intersection, returns_intersection_if_border_point_is_intersected) {
    HalfLine halfLine{Point(0.5, -0.5), Vector(1, 1)};
    auto face = mesh.getBoundary()[0];
    auto intersection = findIntersection(halfLine, face);
    EXPECT_THAT(intersection->halfLine.point.x, DoubleEq(1));
    ASSERT_THAT(intersection->halfLine.point.y, DoubleEq(0));
}


TEST_F(find_intersection, closeset_returns_closest_intersection_if_provided_an_array_of_faces) {
    HalfLine halfLine{Point(0.5, 1.5), Vector(0, -1)};
    auto faces = mesh.getBoundary();
    auto intersection = findClosestIntersection(halfLine, faces);
    EXPECT_THAT(intersection->halfLine.point.x, DoubleEq(0.5));
    ASSERT_THAT(intersection->halfLine.point.y, DoubleEq(1));
}