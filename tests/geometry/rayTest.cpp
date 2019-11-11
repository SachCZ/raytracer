#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "raytracer/geometry/Ray.h"
#include "raytracer/geometry/Point.h"
#include "raytracer/geometry/Vector.h"
#include "raytracer/geometry/Triangle.h"
#include "raytracer/geometry/Mesh.h"

using namespace testing;
using Ray = raytracer::geometry::Ray;
using Point = raytracer::geometry::Point;
using Vector = raytracer::geometry::Vector;
using Triangle = raytracer::geometry::Triangle;
using Mesh = raytracer::geometry::Mesh;

class initialized_ray : public Test {
public:
    Vector direction{1, 2};
    Point startPoint{3, 4};
    Ray ray{startPoint, direction};

};

TEST_F(initialized_ray, has_last_direction) {
    EXPECT_THAT(ray.lastDirection.x, DoubleEq(1));
    ASSERT_THAT(ray.lastDirection.y, DoubleEq(2));
}

TEST_F(initialized_ray, has_one_specific_point) {
    auto point = ray.startPoint;
    EXPECT_THAT(point.x, DoubleEq(3));
    ASSERT_THAT(point.y, DoubleEq(4));
}

TEST_F(initialized_ray, has_last_point) {
    auto point = ray.getLastPoint();
    EXPECT_THAT(point.x, DoubleEq(3));
    ASSERT_THAT(point.y, DoubleEq(4));
}

TEST_F(initialized_ray, can_deal_with_parallel_intersection){
    Triangle triangle({Point(5, 8), Point(4, 6), Point(0, 0)});
    auto point = ray.getClosestIntersection({triangle}).point;
    EXPECT_THAT(point.x, DoubleEq(4));
    EXPECT_THAT(point.y, DoubleEq(6));
}

class two_triangles : public Test {
public:
    Triangle firstTriangle{{{2, 0}, {0, 2}, {1.5, 1}}};
    Triangle secondTriangle{{{0, 0}, {2, 0}, {2, -1}}};
    std::vector<Triangle> triangles = {firstTriangle, secondTriangle};
};

TEST_F(two_triangles, are_intersected_by_ray){
    Ray ray{Point(1, 0), Vector(1, 1)};

    auto intersection = ray.getClosestIntersection(triangles);

    EXPECT_THAT(intersection.point.x, DoubleEq(1.5));
    ASSERT_THAT(intersection.point.y, DoubleEq(0.5));
}

TEST_F(two_triangles, are_intersected_by_ray_for_sure){
    Ray ray{Point(1, 0), Vector(0, -1)};

    auto intersection = ray.getClosestIntersection(triangles);

    EXPECT_THAT(intersection.point.x, DoubleEq(1));
    ASSERT_THAT(intersection.point.y, DoubleEq(-0.5));
}

class ray_on_mesh : public Test {
public:
    Mesh mesh{"./geometry/mesh.stl"};
    Ray ray {Point(-5, 0.0), Vector(1, 0)};

};

TEST_F(ray_on_mesh, deal_with_borders_properly){
    ray.traceThrough(mesh, [](auto intersection){return Vector(1, -1);});

    auto& startPoint = ray.getIntersections().front().point;
    auto& endPoint = ray.getIntersections().back().point;

    EXPECT_THAT(startPoint.x, DoubleEq(-1));
    EXPECT_THAT(startPoint.y, DoubleNear(0, 1e-15));
    EXPECT_THAT(endPoint.x, DoubleNear(0, 1e-15));
    EXPECT_THAT(endPoint.y, DoubleEq(-1));
}

TEST_F(ray_on_mesh, has_correct_intersections_count) {
    ray.traceThrough(mesh, [](auto intersection){return Vector(1, 0);});
    ASSERT_THAT(ray.getIntersections(), SizeIs(43));
}