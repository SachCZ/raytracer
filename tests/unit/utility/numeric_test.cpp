#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utility.h>

using namespace testing;
using namespace raytracer;

TEST(integrate_trapz, gives_correct_result) {
    auto result = raytracer::integrateTrapz([](double x){return 2.0*x + 2.0;}, 2.0, 1.0);
    ASSERT_THAT(result, DoubleEq(7));
}

class initialized_gaussian : public Test {
public:
    IntNormGaussian gaussian{5.0, 2.2, -1};
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