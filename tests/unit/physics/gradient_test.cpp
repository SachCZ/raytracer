#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/matchers.h"

using namespace testing;
using namespace raytracer;

class GradientTest : public Test {
public:
    MfemMesh mesh{SegmentedLine{100.0, 10}, SegmentedLine{100.0, 10}, mfem::Element::TRIANGLE};
    MfemMesh quadMesh{SegmentedLine{100.0, 10}, SegmentedLine{100.0, 10}, mfem::Element::QUADRILATERAL};
    MfemL20Space space{mesh};
    MfemMeshFunction density{space, [](const Point& point){return 12 * point.x - 7*point.y;}};
};

TEST_F(GradientTest, householder_calculates_gradient_correctly){
    auto gradAtPoints = calcHousGrad(mesh, density);
    ASSERT_THAT(gradAtPoints[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}

TEST_F(GradientTest, gradient_can_be_calcualted_using_integral_over_stencil){
    auto gradAtPoints = calcIntegralGrad(quadMesh, density);
    ASSERT_THAT(gradAtPoints[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}

TEST(LinearInterpolationTest, gradinet_can_be_calculated_by_lineary_interpolating){
    Point a{0, 0};
    Point b{1, 0};
    LinInterGrad interGrad{{{&a, Vector{-1, 1}}, {&b, Vector{1, 1}}}};
    Face face{0, {&a, &b}};
    PointOnFace pointOnFace{{0.5, 0}, &face, 0};
    Element element{0, {}, {}};
    auto result = interGrad(pointOnFace, element, element);
    ASSERT_THAT(result, IsSameVector(Vector{0, 1}));
}