#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>

using namespace testing;
using namespace raytracer;


class MfemMeshTest : public Test {
public:
    MfemMesh mesh{DiscreteLine{1.0, 2}, DiscreteLine{1.0, 2}, mfem::Element::Type::QUADRILATERAL};
};


TEST_F(MfemMeshTest, has_proper_boundary) {
    auto boundary = mesh.getBoundary();
    ASSERT_THAT(boundary, SizeIs(8));
}

TEST_F(MfemMeshTest, has_a_way_to_retrive_element_adjacent_to_face_in_direction){
    auto boundary = mesh.getBoundary();
    auto element = mesh.getFaceAdjacentElement(boundary[0], Vector(0, 1));

    ASSERT_THAT(element->getId(), Eq(0));
}