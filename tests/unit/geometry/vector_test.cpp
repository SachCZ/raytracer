#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../support/matchers.h"
#include <geometry.h>

using namespace testing;
using Vector = raytracer::Vector;

class VectorTest : public Test {
public:
    Vector a{3, 4};
    Vector b{-2, 5};

};

TEST_F(VectorTest, has_x) {
    ASSERT_THAT(a.x, DoubleEq(3));
}

TEST_F(VectorTest, has_y) {
    ASSERT_THAT(a.y, DoubleEq(4));
}

TEST_F(VectorTest, vector_can_be_multiplied_by_number) {
    ASSERT_THAT(2 * a, IsSameVector(Vector(6, 8)));
}

TEST_F(VectorTest, vector_number_multiplication_order_does_not_matter) {
    ASSERT_THAT(a * 2, IsSameVector(Vector(6, 8)));
}

TEST_F(VectorTest, vector_has_correct_norm) {
    ASSERT_THAT(a.getNorm(), DoubleEq(5));
}

TEST_F(VectorTest, vectors_can_be_added) {
    ASSERT_THAT(a + b, IsSameVector(Vector(1, 9)));

}

TEST_F(VectorTest, vectors_can_be_subtracted) {
    ASSERT_THAT(a - b, IsSameVector(Vector(5, -1)));

}

TEST_F(VectorTest, vectors_support_dot_product) {
    ASSERT_THAT(a * b, DoubleEq(14));
}