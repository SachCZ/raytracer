#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/geometry_primitives.h>
#include "../matchers.h"

using namespace raytracer;

TEST(ElementTest, centroid_is_calculated_properly){
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
    Point triangleCentroid = getElementCentroid(triangle);
    Point degenerateQuadCentroid = getElementCentroid(degenerateQuad);
    ASSERT_THAT(triangleCentroid, IsSamePoint(degenerateQuadCentroid));
}