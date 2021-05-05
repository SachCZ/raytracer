#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../../support/matchers.h"
#include "../../support/mocks.h"

using namespace testing;
using namespace raytracer;


TEST(StopAtDensityTest, returns_true_if_density_at_element_is_bigger_than_given_density_and_vice_versa) {
    MeshFunctionMock density;
    Element element{0, {}, {}};
    Density criticalDensity{calcCritDens(Length{1315e-7})};
    StopAtDensity stopAtCritical{density, criticalDensity};


    density.setValue(element, 1.1 * criticalDensity.asDouble);
    EXPECT_THAT(stopAtCritical(element), true);
    density.setValue(element, 0.9 * criticalDensity.asDouble);
    ASSERT_THAT(stopAtCritical(element), false);
}

TEST(DontStopTest, returns_always_false) {
    ASSERT_THAT(dontStop(Element{0, {}, {}}), false);
}

TEST(ContinueStraightTest, ContinueStraight_returns_always_the_same_direction) {
    ContinueStraight continueStraight;
    auto result = continueStraight(PointOnFace{}, Vector{0.3, -2});

    ASSERT_THAT(result.value(), IsSameVector(Vector{0.3, -2}));
}


class SnellsLawTest : public Test {
protected:
    void SetUp() override {
        snellsLaw.setGradCalc(gradient);
        auto face = mesh.getElements()[0]->getFaces()[1];
        pointOnFace = {{0, 0.1}, face, 0};
    }

public:
    MfemMesh mesh{{-1.0, 1.0, 2},
                  {0.0,  1.0, 1}};
    MeshFunctionMock refractIndex;
    Length wavelength{1315e-7};
    ConstantGradient gradient{Vector{1, 0}};
    SnellsLaw snellsLaw{&mesh, &refractIndex};
    PointOnFace pointOnFace;
};

TEST_F(SnellsLawTest, snells_law_bends_the_ray_as_expected) {
    auto elements = mesh.getElements();

    refractIndex.setValue(*elements[0], calcRefractIndex(0, wavelength, 0));
    refractIndex.setValue(*elements[1], calcRefractIndex(3.0 / 4.0 * 6.447e20, wavelength, 0));

    auto newDirection = snellsLaw(
            pointOnFace,
            Vector{1, sqrt(3) / 3}
    );

    ASSERT_THAT(newDirection.value(), IsSameVector(Vector{0.0078023764920336358, 0.99996956099727208}));
}

TEST_F(SnellsLawTest, reflects_ray_as_expected) {
    auto elements = mesh.getElements();

    refractIndex.setValue(*elements[0], 1);
    refractIndex.setValue(*elements[1], 0);

    auto newDirection = snellsLaw(
            pointOnFace,
            Vector{1, 1}
    );

    ASSERT_THAT(newDirection.value(), IsSameVector(1 / std::sqrt(2) * Vector{-1, 1}));
}

TEST_F(SnellsLawTest, reflect_on_crit_reflects_while_snell_passes) {
    MeshFunctionMock dens;
    auto elements = mesh.getElements();
    dens.setValue(*elements[0], 1);
    dens.setValue(*elements[1], 10);
    refractIndex.setValue(*elements[0], 1);
    refractIndex.setValue(*elements[1], 1);

    Marker marker;
    ReflectOnCritical reflectOnCritical(mesh, dens, 5, &marker);
    reflectOnCritical.setGradCalc(gradient);

    auto snellDirection = snellsLaw(pointOnFace, Vector{1, 0});
    auto reflectDirection = reflectOnCritical(pointOnFace, Vector{1, 0});

    EXPECT_TRUE(marker.isMarked(pointOnFace));
    EXPECT_THAT(snellDirection.value(), IsSameVector(Vector{1, 0}));
    ASSERT_THAT(reflectDirection.value(), IsSameVector(Vector{-1, 0}));
}

TEST(ReflectAtAxisTest, reflects_ray_on_axis_of_symmetry) {
    auto reflectAtAxis = [](const PointOnFace &pointOnFace, const Vector &dir) {
        if (pointOnFace.point.x >= 0.0) {
            auto newDir = dir;
            newDir.x = -dir.x;
            return tl::optional<Vector>{newDir};
        } else {
            return tl::optional<Vector>{};
        }
    };
    Point pointA{0, 0};
    Point pointD{0, 1};
    Face face{0, {&pointD, &pointA}};
    PointOnFace pointOnFace{};
    pointOnFace.face = &face;
    pointOnFace.point = Point(0, 0.1);
    auto newDirection = reflectAtAxis(
            pointOnFace,
            Vector{1, 1}
    );

    ASSERT_THAT(newDirection.value(), IsSameVector(Vector{-1, 1}));
}

TEST(ReflectAtAxisTest, relfects_even_in_last_cell) {
    MfemMesh mesh(SegmentedLine{0.0, 1.0, 1}, SegmentedLine{0.0, 1.0, 1});
    MeshFunctionMock refractIndex{1.0};
    auto reflectAtAxis = [](const PointOnFace &pointOnFace, const Vector &dir) {
        if (pointOnFace.point.y >= 1.0) {
            auto newDir = dir;
            newDir.y = -dir.y;
            return tl::optional<Vector>{newDir};
        } else {
            return tl::optional<Vector>{};
        }
    };
    auto intersections = findIntersections(
            mesh,
            {Ray{Point(-0.5, 0), Vector{1, 1}}},
            {reflectAtAxis, ContinueStraight{}},
            intersectStraight,
            dontStop
    );
    ASSERT_THAT((*intersections.rbegin()->rbegin()).pointOnFace.point, IsSamePoint(Point{1.0, 0.5}));
}

