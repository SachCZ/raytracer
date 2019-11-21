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

    std::vector<Point> points{
        Point(0, 0), //0
        Point(0, 1), //1
        Point(0, 2), //2
        Point(0, 3), //3
        Point(1, 0), //4
        Point(1, 1), //5
        Point(1, 2), //6
        Point(1, 3), //7
        Point(2, 0), //8
        Point(2, 1), //9
        Point(2, 2), //10
        Point(2, 3), //11
        Point(3, 0), //12
        Point(3, 1), //13
        Point(3, 2), //14
        Point(3, 3), //15
    };


    std::vector<std::vector<size_t>> quadIndexes{
        {0, 4, 5, 1},
        {4, 8, 9, 5},
        {8, 12, 13, 9},

        {1, 5, 6, 2},
        {5, 9, 10, 6},
        {9, 13, 14, 10},

        {2, 6, 7, 3},
        {6, 10, 11, 7},
        {10, 14, 15, 11},
    };
    Mesh mesh{points, quadIndexes};
};

TEST_F(initialized_mesh, has_proper_boundary) {
    auto boundary = mesh.boundary;
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

TEST_F(mesh_from_file, has_correct_quad_count) {
    ASSERT_THAT(mesh.getFacesCount(), Eq(255));
}