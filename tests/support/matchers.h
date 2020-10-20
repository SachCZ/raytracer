#ifndef RAYTRACER_MATCHERS_H
#define RAYTRACER_MATCHERS_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

MATCHER_P(IsSamePoint, point, "") {
    *result_listener << "where expected is: " << point;
    return (arg - point).getNorm() < 1e-10;
}

MATCHER_P(IsSameVector, vector, "") {
    *result_listener << "where expected is: " << vector;
    return (arg - vector).getNorm() < 1e-10;
}


#endif //RAYTRACER_MATCHERS_H
