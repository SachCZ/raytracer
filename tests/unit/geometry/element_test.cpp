#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <geometry.h>
#include "../../support/matchers.h"

using namespace raytracer;
using namespace testing;

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
    Element triangle(1, {&a, &b, &e}, {&A, &B, &C});
    Element degenerateQuad(2, {&a, &b, &nullFace, &e}, {&A, &B, &C, &C});
    Point triangleCentroid = getElementCentroid(triangle);
    Point degenerateQuadCentroid = getElementCentroid(degenerateQuad);
    ASSERT_THAT(triangleCentroid, IsSamePoint(degenerateQuadCentroid));
}

TEST(ElementTest, volume_is_calculated_properly){
    Point A{-3, -2};
    Point B{-1, 4};
    Point C{6, 1};
    Point D{3, 10};
    Point E{-4, 9};

    Face a(0, {&A, &B});
    Face b(1, {&B, &C});
    Face c(2, {&C, &D});
    Face d(3, {&D, &E});
    Face e(4, {&E, &A});

    Element concavePolygon(0, {&a, &b, &c, &d, &e}, {&A, &B, &C, &D, &E});
    auto area = getElementVolume(concavePolygon);
    ASSERT_THAT(area, DoubleEq(60));
}
