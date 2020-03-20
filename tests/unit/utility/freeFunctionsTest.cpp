#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/utility/FreeFunctions.h>

using namespace testing;

TEST(integrate_trapz, gives_correct_result) {
    auto result = raytracer::integrateTrapz([](double x){return 2.0*x + 2.0;}, 2.0, 1.0);
    ASSERT_THAT(result, DoubleEq(7));
}