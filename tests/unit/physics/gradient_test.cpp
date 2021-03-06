#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/matchers.h"

using namespace testing;
using namespace raytracer;

class GradientTest : public Test {
public:
    MfemMesh quadMesh{SegmentedLine{0.0, 100.0, 10}, SegmentedLine{0.0, 100.0, 10}, mfem::Element::QUADRILATERAL};
};

TEST(HouseGradientTest, householder_calculates_gradient_correctly) {
    MfemMesh mesh{SegmentedLine{0.0, 100.0, 10}, SegmentedLine{0.0, 100.0, 10}, mfem::Element::TRIANGLE};
    MfemL20Space space{mesh};
    MfemMeshFunction density{space, [](const Point &point) { return 12 * point.x - 7 * point.y; }};
    auto VectorField = calcHousGrad(mesh, density);
    ASSERT_THAT(VectorField[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}

TEST(IntegralGradientTest, gradient_can_be_calcualted_using_integral_over_stencil) {
    MfemMesh mesh{SegmentedLine{0.0, 100.0, 10}, SegmentedLine{0.0, 100.0, 10}, mfem::Element::QUADRILATERAL};
    MfemL20Space space{mesh};
    MfemMeshFunction density{space, [](const Point &point) { return 12 * point.x - 7 * point.y;}};
    auto VectorField = calcIntegralGrad(mesh, density);
    ASSERT_THAT(VectorField[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}

TEST(LinearInterpolationTest, gradinet_can_be_calculated_by_lineary_interpolating) {
    Point a{0, 0};
    Point b{1, 0};
    VectorField gradAtPoints{{&a, Vector{-1, 1}}, {&b, Vector{1, 1}}};
    LinInterGrad interGrad{gradAtPoints};
    Face face{0, {&a, &b}};
    PointOnFace pointOnFace{{0.5, 0}, &face, 0};
    auto result = interGrad.get(pointOnFace).value();
    ASSERT_THAT(result, IsSameVector(Vector{0, 1}));
}