#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>


using namespace testing;
using namespace raytracer;


class MfemMeshFunctionTest : public Test {
public:
    MfemMesh mesh{SegmentedLine{1.0, 2}, SegmentedLine{1.0, 2}, mfem::Element::Type::QUADRILATERAL};
    MfemL20Space finiteElementSpace{mesh};
    MfemMeshFunction meshFunction{finiteElementSpace, [](const Point& point){return 12.8e20 * point.x;}};
};

TEST_F(MfemMeshFunctionTest, MfemMeshFunction_value_can_be_retrieved_given_an_element) {
    auto boundary = mesh.getBoundary();
    auto element = mesh.getFaceAdjacentElement(boundary[0], Vector(0, 1));

    auto value = meshFunction.getValue(*element);
    ASSERT_THAT(value, DoubleEq(12.8e20 / 4));
}

TEST_F(MfemMeshFunctionTest, MfemMeshFunction_value_can_be_set_given_an_element) {
    auto boundary = mesh.getBoundary();
    auto element = mesh.getFaceAdjacentElement(boundary[0], Vector(0, 1));

    meshFunction.addValue(*element, 12.8e20 / 4);
    auto value = meshFunction.getValue(*element);
    ASSERT_THAT(value,  DoubleEq(12.8e20 / 2));
}

TEST_F(MfemMeshFunctionTest, MfemMeshFunction_can_be_constructed_using_grid_function){
    mfem::GridFunction gridFunction(&finiteElementSpace.getSpace());
    gridFunction = 42;
    MfemMeshFunction newMeshFunction(finiteElementSpace, gridFunction);
    ASSERT_THAT(newMeshFunction.getValue(Element{0, {}, {}}), DoubleEq(42));
}