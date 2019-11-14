#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/Mesh.h>
#include <raytracer/utility/AdjacencyList.h>
#include <raytracer/geometry/Point.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using AdjacencyList = raytracer::utility::AdjacencyList;
using Mesh = raytracer::geometry::Mesh;
using Triangle = raytracer::geometry::Triangle;
using Point = raytracer::geometry::Point;
using Vector = raytracer::geometry::Vector;

class initialized_mesh : public Test {
public:
    std::vector<Triangle > triangles{
        Triangle({{0, 0}, {0, 1}, {1, 1}}),
        Triangle({{0, 0}, {1, 0}, {1, 1}}),
        Triangle({{1, 0}, {1, 1}, {2, 1}}),
        Triangle({{1, 0}, {2, 1}, {2, 0}}),
        Triangle({{0, 1}, {0, 2}, {1, 2}}),
        Triangle({{0, 1}, {1, 1}, {1, 2}}),
        Triangle({{1, 1}, {1, 2}, {2, 2}}),
        Triangle({{1, 1}, {2, 1}, {2, 2}})
    };
    Mesh mesh{triangles};
};

TEST_F(initialized_mesh, has_proper_boundary) {
    auto boundary = mesh.getBoundary();
    ASSERT_THAT(boundary, SizeIs(6));
}

TEST_F(initialized_mesh, entries_are_adjacent) {
    const auto triangle = mesh[2];
    const auto borderTriangle = mesh[3];

    EXPECT_THAT(mesh.getAdjacent(borderTriangle), SizeIs(1));
    ASSERT_THAT(mesh.getAdjacent(triangle), SizeIs(3));
}

class mesh_from_file : public Test {
public:
    Mesh mesh{"./geometry/mesh.stl"};
};

TEST_F(mesh_from_file, has_correct_triangle_count) {
    ASSERT_THAT(mesh.getFacesCount(), Eq(882));
}