#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "raytracer/geometry/Ray.h"
#include "raytracer/geometry/Point.h"
#include "raytracer/geometry/Vector.h"
#include "raytracer/geometry/Quadrilateral.h"
#include "raytracer/geometry/Mesh.h"

using namespace testing;
using Ray = raytracer::geometry::Ray;
using Point = raytracer::geometry::Point;
using Vector = raytracer::geometry::Vector;
using Quad = raytracer::geometry::Quadrilateral;
using Mesh = raytracer::geometry::Mesh;
using RayState = raytracer::geometry::RayState;

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

TEST_F(initialized_ray, can_deal_with_parallel_intersection){
    std::vector<Point> points = {Point(4, 4), Point(8, 4), Point(8, 8), Point(4, 8)};
    Quad quad({&points[0], &points[1], &points[2], &points[3]});
    auto point = ray.getClosestIntersection({quad})[0].point;
    EXPECT_THAT(point.x, DoubleEq(4));
    EXPECT_THAT(point.y, DoubleEq(6));
}

class two_quads : public Test {
public:
    void SetUp() override {
        points.emplace_back(std::make_unique<Point>(0, 0));
        points.emplace_back(std::make_unique<Point>(1, 0));
        points.emplace_back(std::make_unique<Point>(1, 1));
        points.emplace_back(std::make_unique<Point>(0, 1));
        points.emplace_back(std::make_unique<Point>(2, 0));
        points.emplace_back(std::make_unique<Point>(2, 1));

        std::vector<const Point*> firstTemp{{points[0].get(), points[1].get(), points[2].get(), points[3].get()}};
        std::vector<const Point*> secondTemp{{points[1].get(), points[4].get(), points[5].get(), points[2].get()}};

        firstQuad = std::make_unique<Quad>(firstTemp);
        secondQuad = std::make_unique<Quad>(secondTemp);

        quads.emplace_back(*firstQuad);
        quads.emplace_back(*secondQuad);
    }

    std::vector<std::unique_ptr<Point>> points;
    std::unique_ptr<Quad> firstQuad;
    std::unique_ptr<Quad> secondQuad;
    std::vector<Quad> quads;
};

TEST_F(two_quads, are_intersected_by_ray){
    Ray ray{Point(-1, 0.5), Vector(1, 0)};

    auto intersection = ray.getClosestIntersection(quads)[0];

    EXPECT_THAT(intersection.point.x, DoubleEq(0));
    ASSERT_THAT(intersection.point.y, DoubleEq(0.5));
}

TEST_F(two_quads, are_intersected_by_ray_for_sure){
    Ray ray{Point(0.5, 2), Vector(0, -1)};

    auto intersection = ray.getClosestIntersection(quads)[0];

    EXPECT_THAT(intersection.point.x, DoubleEq(0.5));
    ASSERT_THAT(intersection.point.y, DoubleEq(1));
}

class ray_on_mesh : public Test {
public:
    Mesh mesh{"./geometry/mesh.vtk"};
    Ray ray {Point(-5, 0.0), Vector(1, 0)};

};

TEST_F(ray_on_mesh, deal_with_borders_properly){
    ray.traceThrough(
            mesh,
            [](const RayState& rayState){return Vector(1, -1);},
            [](const Quad & quad){return false;});

    auto& startPoint = ray.getIntersections().front().point;
    auto& endPoint = ray.getIntersections().back().point;

    EXPECT_THAT(startPoint.x, DoubleEq(-1));
    EXPECT_THAT(startPoint.y, DoubleNear(0, 1e-15));
    EXPECT_THAT(endPoint.x, DoubleNear(0, 1e-15));
    EXPECT_THAT(endPoint.y, DoubleEq(-1));
}

TEST_F(ray_on_mesh, has_correct_intersections_count) {
    ray.traceThrough(
            mesh,
            [](const RayState& rayState){return Vector(1, 0);},
            [](const Quad& quad){return false;});
    ASSERT_THAT(ray.getIntersections(), SizeIs(16));
}