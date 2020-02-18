#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/Mesh.h>
#include <raytracer/geometry/Element.h>
#include <raytracer/geometry/Point.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using namespace raytracer::geometry;


class initialized_mesh : public Test {
public:
    initialized_mesh() {
        DiscreteLine sideA{1.0, 2};
        DiscreteLine sideB{1.0, 2};
        mfemMesh = std::move(constructRectangleMesh(sideA, sideB));
        mesh = std::make_unique<Mesh>(mfemMesh.get());
    }
    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<Mesh> mesh;
};


TEST_F(initialized_mesh, has_proper_boundary) {
    auto boundary = mesh->getBoundary();
    ASSERT_THAT(boundary, SizeIs(8));
}

TEST_F(initialized_mesh, has_a_way_to_retrive_element_adjacent_to_face_in_direction){
    auto boundary = mesh->getBoundary();
    auto element = mesh->getAdjacentElement(boundary[0], HalfLine{Point(0.4, 0), Vector(0, 1)});
    auto faces = element->getFaces();
    auto normal = faces[2]->getNormal();

    EXPECT_THAT(normal.x, DoubleEq(0));
    EXPECT_THAT(normal.y, DoubleEq(0.5));

    //Double check
    element = mesh->getAdjacentElement(faces[1], HalfLine{Point(0.4, 0), Vector(0, -1)});
    faces = element->getFaces();
    normal = faces[0]->getNormal();

    EXPECT_THAT(normal.x, DoubleEq(0));
    EXPECT_THAT(normal.y, DoubleEq(-0.5));
}

TEST_F(initialized_mesh, has_a_way_to_retrive_faces_adjacent_to_vertex){
    auto boundary = mesh->getBoundary();
    auto element = mesh->getAdjacentElement(boundary[0], HalfLine{Point(0.4, 0), Vector(0, 1)});
    auto face = element->getFaces()[2];
    auto point = face->getPoints()[0];
    auto elements = mesh->getAdjacentElements(point);
    ASSERT_THAT(elements, SizeIs(4));
}