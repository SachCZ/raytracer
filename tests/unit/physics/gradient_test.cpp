#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/matchers.h"

using namespace testing;
using namespace raytracer;

class GradientTest : public Test {
public:
    MfemMesh mesh{SegmentedLine{100.0, 10}, SegmentedLine{100.0, 10}, mfem::Element::TRIANGLE};
    MfemL20Space space{mesh};
    MfemMeshFunction density{space, [](const Point& point){return 12 * point.x - 7*point.y;}};
};

TEST_F(GradientTest, householder_calculates_gradient_correctly){
    auto gradientAtPoints = calcHousGrad(mesh, density);
    ASSERT_THAT(gradientAtPoints[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}