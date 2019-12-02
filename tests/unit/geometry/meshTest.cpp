#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/Mesh.h>
#include <raytracer/geometry/Point.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using namespace raytracer::geometry;


class initialized_mesh : public Test {
public:
    DiscreteLine sideA{1.0, 2};
    DiscreteLine sideB{1.0, 2};

    Mesh mesh{sideA, sideB};

};

TEST_F(initialized_mesh, has_proper_boundary) {
    auto boundary = mesh.getBoundary();
    ASSERT_THAT(boundary, SizeIs(8));
}

TEST_F(initialized_mesh, has_a_way_to_retrive_element_adjacent_to_face_in_direction){
    auto boundary = mesh.getBoundary();
    auto element = mesh.getAdjacentElement(boundary[0], Vector(0, 1));
    auto faces = (*element).getFaces();
    auto normal = faces[2].getNormal();

    EXPECT_THAT(normal.x, DoubleEq(0));
    EXPECT_THAT(normal.y, DoubleEq(0.5));

    //Double check
    element = mesh.getAdjacentElement(faces[1], Vector(0, -1));
    faces = (*element).getFaces();
    normal = faces[0].getNormal();

    EXPECT_THAT(normal.x, DoubleEq(0));
    EXPECT_THAT(normal.y, DoubleEq(-0.5));
}