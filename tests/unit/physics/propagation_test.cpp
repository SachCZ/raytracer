#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include "../matchers.h"
#include "../mocks.h"

using namespace testing;
using namespace raytracer;


TEST(StopAtDensityTest, returns_true_if_density_at_element_is_bigger_than_given_density_and_vice_versa) {
    MeshFunctionMock density;
    Element element{0, {}};
    Density criticalDensity{ClassicCriticalDensity{}.getCriticalDensity(Length{1315e-7})};
    StopAtDensity stopAtCritical{density, criticalDensity};


    density.setValue(element, 1.1 * criticalDensity.asDouble);
    EXPECT_THAT(stopAtCritical(element), true);
    density.setValue(element, 0.9 * criticalDensity.asDouble);
    ASSERT_THAT(stopAtCritical(element), false);
}

TEST(DontStopTest, returns_always_false) {
    DontStop dontStop;

    ASSERT_THAT(dontStop(Element{0, {}}), false);
}

TEST(ContinueStraightTest, ContinueStraight_returns_always_the_same_direction) {
    ContinueStraight continueStraight;
    auto result = continueStraight(PointOnFace{}, Vector{0.3, -2}, Element{0, {}}, Element{1, {}});

    ASSERT_THAT(result, IsSameVector(Vector{0.3, -2}));
}

TEST(SnellsLawTest, snells_law_bends_the_ray_as_expected) {
    Element previousElement{0, {}};
    Element nextElement{1, {}};

    MeshFunctionMock density;
    MeshFunctionMock temperature;
    MeshFunctionMock ionization;

    density.setValue(previousElement, 0);
    density.setValue(nextElement, 3.0 / 4.0 * 6.447e20);
    temperature.setValue(previousElement, 0);
    temperature.setValue(nextElement, 0);
    ionization.setValue(previousElement, 0);
    ionization.setValue(nextElement, 0);

    ConstantGradient gradient{Vector{1, 0}};
    ConstantFrequency frequency{Frequency{0}};
    ColdPlasma coldPlasma;
    SnellsLaw snellsLaw{
        density,
        temperature,
        ionization,
        gradient,
        frequency,
        coldPlasma,
        Length{1315e-7}
    };

    Point pointA{0, 0};
    Point pointD{0, 1};
    Face face{0, {&pointD, &pointA}};
    PointOnFace pointOnFace{};
    pointOnFace.face = &face;
    pointOnFace.point = Point(0, 0.1);
    auto newDirection = snellsLaw(
            pointOnFace,
            Vector{1, sqrt(3) / 3},
            previousElement,
            nextElement
    );

    ASSERT_THAT(newDirection, IsSameVector(Vector{0.0078023764920336358, 0.99996956099727208}));
}