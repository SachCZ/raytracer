#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/Mesh.h>
#include <raytracer/geometry/Element.h>
#include <raytracer/geometry/MeshFunction.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using namespace raytracer::geometry;

double density(const mfem::Vector &x) {
    return 12.8e20 * x(0);
}

class mesh_function : public Test {
public:
    mesh_function() {
        DiscreteLine sideA{1.0, 2};
        DiscreteLine sideB{1.0, 2};
        mfemMesh = std::move(constructRectangleMesh(sideA, sideB));
        mesh = std::make_unique<Mesh>(mfemMesh.get());


        finiteElementSpace = std::make_unique<mfem::FiniteElementSpace>(mfemMesh.get(), &finiteElementCollection);
        densityGridFunction = std::make_unique<mfem::GridFunction>(finiteElementSpace.get());

        mfem::FunctionCoefficient densityFunctionCoefficient{density};
        densityGridFunction->ProjectCoefficient(densityFunctionCoefficient);
        meshFunction = std::make_unique<MeshFunction>(*densityGridFunction, *finiteElementSpace);
    }

    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<Mesh> mesh;

    mfem::L2_FECollection finiteElementCollection{0, 2};
    std::unique_ptr<mfem::FiniteElementSpace> finiteElementSpace;

    std::unique_ptr<mfem::GridFunction> densityGridFunction;
    std::unique_ptr<MeshFunction> meshFunction;
};

TEST_F(mesh_function, value_can_be_retrieved_given_an_element) {
    auto boundary = mesh->getBoundary();
    auto element = mesh->getAdjacentElement(boundary[0], Vector(0, 1));

    auto value = (*meshFunction)[*element];
    ASSERT_THAT(value, DoubleEq(12.8e20 / 4));
}

TEST_F(mesh_function, value_can_be_set_given_an_element) {
    auto boundary = mesh->getBoundary();
    auto element = mesh->getAdjacentElement(boundary[0], Vector(0, 1));

    (*meshFunction)[*element] += 12.8e20 / 4;
    auto value = (*meshFunction)[*element];
    ASSERT_THAT(value,  DoubleEq(12.8e20 / 2));
}