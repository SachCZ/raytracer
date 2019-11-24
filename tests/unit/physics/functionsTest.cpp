#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/Functions.h>

using namespace testing;
using Gaussian = raytracer::physics::Gaussian;
using LinearFunction = raytracer::physics::LinearFunction;

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

class initialized_linear_function : public Test {
public:
    LinearFunction linearFunction{2, 1};
};

TEST_F(initialized_linear_function, returns_correct_values){
    auto value = linearFunction(1);
    ASSERT_THAT(value, DoubleEq(3));
}