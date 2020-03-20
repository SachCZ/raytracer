#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using Vector = raytracer::Vector;

class initialized_vector : public Test {
public:
    Vector vector{3, 4};
};

TEST_F(initialized_vector, has_x) {
    ASSERT_THAT(vector.x, DoubleEq(3));
}

TEST_F(initialized_vector, has_y) {
    ASSERT_THAT(vector.y, DoubleEq(4));
}

TEST_F(initialized_vector, can_be_multiplied_by_number) {
    Vector result = 2 * vector;
    EXPECT_THAT(result.x, DoubleEq(6));
    ASSERT_THAT(result.y, DoubleEq(8));
}

TEST_F(initialized_vector, number_multiplication_order_does_not_matter) {
    Vector result = vector * 2;
    EXPECT_THAT(result.x, DoubleEq(6));
    ASSERT_THAT(result.y, DoubleEq(8));
}

TEST_F(initialized_vector, has_correct_norm) {
    ASSERT_THAT(vector.getNorm(), DoubleEq(5));
}

class two_vectors : public Test {
public:
    Vector a{2, 5};
    Vector b{-5, 2.2};
};

TEST_F(two_vectors, could_be_added) {
    Vector result = a + b;
    EXPECT_THAT(result.x, DoubleEq(-3));
    ASSERT_THAT(result.y, DoubleEq(7.2));
}

TEST_F(two_vectors, could_be_subtracted) {
    Vector result = a - b;
    EXPECT_THAT(result.x, DoubleEq(7));
    ASSERT_THAT(result.y, DoubleEq(2.8));
}

TEST_F(two_vectors, support_dot_product) {
    double result = a * b;
    ASSERT_THAT(result, DoubleEq(1));
}