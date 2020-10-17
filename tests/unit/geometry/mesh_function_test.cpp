#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>


using namespace testing;
using namespace raytracer;


class MfemMeshFunctionTest : public Test {
protected:
    void SetUp() override {
        meshFunction.setUsingFunction(mesh, [](const Point& point){return 12.8e20 * point.x;});
    }

public:
    MfemMesh mesh{DiscreteLine{1.0, 2}, DiscreteLine{1.0, 2}, mfem::Element::Type::QUADRILATERAL};
    mfem::L2_FECollection finiteElementCollection{0, 2};
    mfem::FiniteElementSpace finiteElementSpace{mesh.getMfemMesh(), &finiteElementCollection};
    MfemMeshFunction meshFunction{finiteElementSpace};
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