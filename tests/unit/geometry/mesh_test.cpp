#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>

using namespace testing;
using namespace raytracer;


class MfemMeshTest : public Test {
public:
    MfemMeshTest() {
        DiscreteLine sideA{1.0, 2};
        DiscreteLine sideB{1.0, 2};
        mfemMesh = std::move(constructMfemMesh(sideA, sideB, mfem::Element::Type::QUADRILATERAL));
        mesh = std::make_unique<MfemMesh>(mfemMesh.get());
    }
    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<MfemMesh> mesh;
};


TEST_F(MfemMeshTest, has_proper_boundary) {
    auto boundary = mesh->getBoundary();
    ASSERT_THAT(boundary, SizeIs(8));
}

TEST_F(MfemMeshTest, has_a_way_to_retrive_element_adjacent_to_face_in_direction){
    auto boundary = mesh->getBoundary();
    auto element = mesh->getFaceAdjacentElement(boundary[0], Vector(0, 1));

    EXPECT_THAT(element->getId(), Eq(0));
}