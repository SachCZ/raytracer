#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/matchers.h"

using namespace testing;
using namespace raytracer;



TEST(HouseGradientTest, householder_calculates_gradient_correctly) {
    MfemMesh mesh{SegmentedLine{0.0, 100.0, 10}, SegmentedLine{0.0, 100.0, 10}, mfem::Element::TRIANGLE};
    std::vector<double> density;
    const auto &elements = mesh.getElements();
    std::transform(elements.begin(), elements.end(), std::back_inserter(density), [](const Element *element) {
        auto center = getElementCentroid(*element);
        return 12 * center.x - 7 * center.y;
    });
    auto VectorField = calcHousGrad(mesh, density);
    ASSERT_THAT(VectorField[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}

TEST(IntegralGradientTest, gradient_can_be_calcualted_using_integral_over_stencil) {
    MfemMesh mesh{SegmentedLine{0.0, 100.0, 10}, SegmentedLine{0.0, 100.0, 10}, mfem::Element::QUADRILATERAL};
    std::vector<double> density;
    const auto &elements = mesh.getElements();
    std::transform(elements.begin(), elements.end(), std::back_inserter(density), [](const Element *element) {
        auto center = getElementCentroid(*element);
        return 12 * center.x - 7 * center.y;
    });
    auto VectorField = calcIntegralGrad(mesh, density);
    ASSERT_THAT(VectorField[mesh.getPoints()[27]], IsSameVector(Vector{12, -7}));
}

TEST(LinearInterpolationTest, gradinet_can_be_calculated_by_lineary_interpolating) {
    Point a{0, 0};
    Point b{1, 0};
    VectorField gradAtPoints{{&a, Vector{-1, 1}},
                             {&b, Vector{1, 1}}};
    LinInterGrad interGrad{gradAtPoints};
    Face face{0, {&a, &b}};
    PointOnFace pointOnFace{{0.5, 0}, &face, 0};
    auto result = interGrad.get(pointOnFace).value();
    ASSERT_THAT(result, IsSameVector(Vector{0, 1}));
}