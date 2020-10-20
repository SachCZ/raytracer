#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/matchers.h"

using namespace testing;
using namespace raytracer;

class GradientTest : public Test {
public:
    void SetUp() override {
        density.setUsingFunction(mesh, [](const Point& point){return 12 * point.x - 7*point.y;});
    }

    MfemMesh mesh{SegmentedLine{100.0, 10}, SegmentedLine{100.0, 10}, mfem::Element::TRIANGLE};
    mfem::L2_FECollection l2FiniteElementCollection{0, 2};
    mfem::FiniteElementSpace l2FiniteElementSpace{mesh.getMfemMesh(), &l2FiniteElementCollection};
    MfemMeshFunction density{l2FiniteElementSpace};
};

TEST_F(GradientTest, householder_calculates_gradient_correctly){
    auto gradientAtPoints = getHouseholderGradientAtPoints(mesh, density);
    ASSERT_THAT(gradientAtPoints[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}