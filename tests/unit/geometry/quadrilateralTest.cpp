#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/Quadrilateral.h>

using namespace testing;
using Point = raytracer::geometry::Point;
using Quadrilateral = raytracer::geometry::Quadrilateral;

class initialized_convex_quadrilateral : public Test {
public:
    Quadrilateral quadrilateral{{{0,0}, {1, 0}, {1, 1}, {0, 1}}};
};

TEST_F(initialized_convex_quadrilateral, could_be_triangulated) {
    auto triangles = quadrilateral.getTriangulation();
    ASSERT_THAT(triangles, SizeIs(2));
    /**
     * This test would be overly complicated to write so it just checks the result size is 2
     * not the actual correctness of the result. May not be reliable!
     */
     //TODO write better test

}

class initialized_non_convex_quadrilateral : public Test {
public:
    Quadrilateral quadrilateral{{{0,1}, {0.5, 0.5}, {0, 0}, {1, 0.5}}};
};

TEST_F(initialized_non_convex_quadrilateral, could_be_triangulated) {
    auto triangles = quadrilateral.getTriangulation();
    ASSERT_THAT(triangles, SizeIs(2));
    /**
     * This test would be overly complicated to write so it just checks the result size is 2
     * not the actual correctness of the result. May not be reliable!
     */
    //TODO write better test
    return;
}