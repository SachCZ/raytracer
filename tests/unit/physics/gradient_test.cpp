#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <raytracer/geometry/mesh.h>
#include <raytracer/physics/propagation.h>
#include <raytracer/physics/laser.h>
#include "raytracer/physics/gradient.h"
#include "raytracer/utility/numeric.h"
#include "../matchers.h"
#include "refraction.h"
#include "termination.h"

using namespace testing;
using namespace raytracer;

class GradientTest : public Test {
    static double density(const mfem::Vector &x) {
        return 12 * x(0) - 7 * x(1);
    }

public:
    void SetUp() override {
        mfem::FunctionCoefficient densityFunctionCoefficient(GradientTest::density);
        densityGridFunction.ProjectCoefficient(densityFunctionCoefficient);
    }




    DiscreteLine side{100, 10};
    std::unique_ptr<mfem::Mesh> mfemMesh = constructMfemMesh(side, side, mfem::Element::TRIANGLE);

    mfem::L2_FECollection l2FiniteElementCollection{0, 2};
    mfem::FiniteElementSpace l2FiniteElementSpace{mfemMesh.get(), &l2FiniteElementCollection};
    mfem::GridFunction densityGridFunction{&l2FiniteElementSpace};
    MfemMeshFunction densityMeshFunction{densityGridFunction, l2FiniteElementSpace};
    MfemMesh mesh{mfemMesh.get()};
};

TEST_F(GradientTest, householder_calculates_gradient_correctly){
    auto gradientAtPoints = getHouseholderGradientAtPoints(mesh, densityMeshFunction);
    ASSERT_THAT(gradientAtPoints[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}