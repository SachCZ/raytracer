#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/utility/FreeFunctions.h>
#include "../matchers.h"

using namespace testing;
using namespace raytracer;

TEST(integrate_trapz, gives_correct_result) {
    auto result = raytracer::integrateTrapz([](double x){return 2.0*x + 2.0;}, 2.0, 1.0);
    ASSERT_THAT(result, DoubleEq(7));
}

TEST(polygon_centroid, is_calculated_properly){
    Point A{0, 0};
    Point B{1, 0};
    Point C{1, 1};
    Point D{0, 1};
    Face a(0, {&A, &B});
    Face b(1, {&B, &C});
    Face c(2, {&C, &D});
    Face d(3, {&D, &A});
    Face e(4, {&C, &A});
    Face nullFace(5, {&C, &C});
    Element triangle(1, {&a, &b, &e});
    Element degenerateQuad(2, {&a, &b, &nullFace, &e});
    Point triangleCentroid = getPolygonCentroid(triangle);
    Point degenerateQuadCentroid = getPolygonCentroid(degenerateQuad);
    ASSERT_THAT(triangleCentroid, IsSamePoint(degenerateQuadCentroid));
}

class initialized_gaussian : public Test {
public:
    Gaussian gaussian{5.0, 2.2, -1};
};

TEST_F(initialized_gaussian, value_at_half_FWHM_is_half_maximum) {
    auto value = gaussian(-3.5);
    ASSERT_THAT(value, DoubleEq(gaussian(-1) / 2.0));
}

TEST_F(initialized_gaussian, is_symetric_around_center) {
    auto leftValue = gaussian(-5.2);
    auto rightValue = gaussian(3.2);
    ASSERT_THAT(leftValue, DoubleEq(rightValue));
}