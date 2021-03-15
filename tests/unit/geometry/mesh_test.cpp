#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>
#include "matchers.h"

using namespace testing;
using namespace raytracer;


class MfemMeshTest : public Test {
public:
    MfemMesh mesh{SegmentedLine{0.0, 1.0, 2}, SegmentedLine{0.0, 1.0, 2}, mfem::Element::Type::QUADRILATERAL};
};


TEST_F(MfemMeshTest, has_proper_boundary) {
    auto boundary = mesh.getBoundary();
    ASSERT_THAT(boundary, SizeIs(8));
}

TEST_F(MfemMeshTest, has_a_way_to_retrive_element_adjacent_to_face_in_direction){
    auto boundary = mesh.getBoundary();
    auto element = mesh.getFaceDirAdjElement(boundary[0], Vector(0, 1));

    ASSERT_THAT(element->getId(), Eq(0));
}

TEST_F(MfemMeshTest, has_a_way_to_retrive_ordered_adjacent_faces){
    auto adjacentFaces = mesh.getPointAdjOrderedFaces(mesh.getInnerPoints()[0]);
    ASSERT_THAT(adjacentFaces, SizeIs(4));
}

TEST_F(MfemMeshTest, has_a_way_to_retrive_ordered_adjacent_elements){
    auto adjacentElements = mesh.getPointAdjOrderedElements(mesh.getInnerPoints()[0]);
    EXPECT_THAT(adjacentElements[0]->getId(), Eq(3));
    EXPECT_THAT(adjacentElements[1]->getId(), Eq(2));
    EXPECT_THAT(adjacentElements[2]->getId(), Eq(1));
    ASSERT_THAT(adjacentElements[3]->getId(), Eq(0));
}

TEST_F(MfemMeshTest, has_a_way_to_retrive_ordered_adjacent_points){
    auto adjacentPoints = mesh.getPointAdjOrderedPoints(mesh.getInnerPoints()[0]);
    EXPECT_THAT(*adjacentPoints[0], IsSamePoint(Point{0.5, 0.0}));
    EXPECT_THAT(*adjacentPoints[1], IsSamePoint(Point{1.0, 0.5}));
    EXPECT_THAT(*adjacentPoints[2], IsSamePoint(Point{0.5, 1.0}));
    ASSERT_THAT(*adjacentPoints[3], IsSamePoint(Point{0.0, 0.5}));
}