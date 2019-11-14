#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/Mesh.h>
#include <raytracer/utility/AdjacencyList.h>
#include <raytracer/geometry/Point.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using AdjacencyList = raytracer::utility::AdjacencyList;
using Mesh = raytracer::geometry::Mesh;
using Quad = raytracer::geometry::Quadrilateral;
using Point = raytracer::geometry::Point;
using Vector = raytracer::geometry::Vector;

class initialized_mesh : public Test {
public:
    std::vector<Quad > quads{
        Quad({{0, 0}, {1, 0}, {1, 1}, {0, 1}}),
        Quad({{1, 0}, {2, 0}, {2, 1}, {1, 1}}),
        Quad({{2, 0}, {3, 0}, {3, 1}, {2, 1}}),

        Quad({{0, 1}, {1, 1}, {1, 2}, {0, 2}}),
        Quad({{1, 1}, {2, 1}, {2, 2}, {1, 2}}),
        Quad({{2, 1}, {3, 1}, {3, 2}, {2, 2}}),

        Quad({{0, 2}, {1, 2}, {1, 3}, {0, 3}}),
        Quad({{1, 2}, {2, 2}, {2, 3}, {1, 3}}),
        Quad({{2, 2}, {3, 2}, {3, 3}, {2, 3}})
    };
    Mesh mesh{quads};
};

TEST_F(initialized_mesh, has_proper_boundary) {
    auto boundary = mesh.getBoundary();
    ASSERT_THAT(boundary, SizeIs(8));
}

TEST_F(initialized_mesh, entries_are_adjacent) {
    const auto quad = mesh[4];
    const auto borderQuad = mesh[0];

    EXPECT_THAT(mesh.getAdjacent(borderQuad), SizeIs(2));
    ASSERT_THAT(mesh.getAdjacent(quad), SizeIs(4));
}

class mesh_from_file : public Test {
public:
    Mesh mesh{"./geometry/mesh.vtk"};
};

TEST_F(mesh_from_file, has_correct_triangle_count) {
    ASSERT_THAT(mesh.getFacesCount(), Eq(255));
}