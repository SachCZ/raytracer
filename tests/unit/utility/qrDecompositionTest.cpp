#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/utility/QRDecomposition.h>
#include <raytracer/geometry/Vector.h>

using namespace testing;
using namespace rosetta;

double in[][3] = {
        { 12, -51,   4},
        {  6, 167, -68},
        { -4,  24, -41},
        { -1,   1,   0},
        {  2,   0,   3},
};

TEST(householder_algorithm, gives_correct_result) {
    Matrix A(in);
    Matrix Q, R;

    // compute QR decompostion
    householder(A, R, Q);
    Q.trim_columns(3);
    R.trim_rows(3);

    // compare Q*R to the original matrix A
    Matrix A_check;
    A_check.mult(Q, R);

    // compute L2 norm ||A-A_check||^2
    double l2 = matrix_compare(A,A_check);
    ASSERT_THAT(l2, Lt(1e-12));
}

double A_in[][3] = {
        {-1, -1, 1},
        {0, 3, 1},
        {0, 0, 2}
};

double b_in[][1] = {
        {7},
        {8},
        {4}
};

double x_check_in[][1] = {
        {-7},
        {2},
        {2}
};

TEST(forward_substitute_algorithm, gives_correct_result){
    Matrix A(A_in);
    Matrix x(3, 1);
    Matrix b(b_in);
    Matrix x_check(x_check_in);

    x.forward_substitute(A, b);

    double l2 = matrix_compare(x, x_check);
    ASSERT_THAT(l2, Lt(1e-12));
}