#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>


using namespace testing;
using namespace raytracer;


class MfemMeshFunctionTest : public Test {
    static double density(const mfem::Vector &x) {
        return 12.8e20 * x(0);
    }
public:
    MfemMeshFunctionTest() {
        DiscreteLine sideA{1.0, 2};
        DiscreteLine sideB{1.0, 2};
        mfemMesh = std::move(constructMfemMesh(sideA, sideB, mfem::Element::Type::QUADRILATERAL));
        mesh = std::make_unique<MfemMesh>(mfemMesh.get());


        finiteElementSpace = std::make_unique<mfem::FiniteElementSpace>(mfemMesh.get(), &finiteElementCollection);
        densityGridFunction = std::make_unique<mfem::GridFunction>(finiteElementSpace.get());

        mfem::FunctionCoefficient densityFunctionCoefficient{MfemMeshFunctionTest::density};
        densityGridFunction->ProjectCoefficient(densityFunctionCoefficient);
        meshFunction = std::make_unique<MfemMeshFunction>(*densityGridFunction, *finiteElementSpace);
    }

    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<MfemMesh> mesh;

    mfem::L2_FECollection finiteElementCollection{0, 2};
    std::unique_ptr<mfem::FiniteElementSpace> finiteElementSpace;

    std::unique_ptr<mfem::GridFunction> densityGridFunction;
    std::unique_ptr<MfemMeshFunction> meshFunction;
};

TEST_F(MfemMeshFunctionTest, MfemMeshFunction_value_can_be_retrieved_given_an_element) {
    auto boundary = mesh->getBoundary();
    auto element = mesh->getFaceAdjacentElement(boundary[0], Vector(0, 1));

    auto value = meshFunction->getValue(*element);
    ASSERT_THAT(value, DoubleEq(12.8e20 / 4));
}

TEST_F(MfemMeshFunctionTest, MfemMeshFunction_value_can_be_set_given_an_element) {
    auto boundary = mesh->getBoundary();
    auto element = mesh->getFaceAdjacentElement(boundary[0], Vector(0, 1));

    meshFunction->addValue(*element, 12.8e20 / 4);
    auto value = meshFunction->getValue(*element);
    ASSERT_THAT(value,  DoubleEq(12.8e20 / 2));
}