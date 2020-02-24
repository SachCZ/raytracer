#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "raytracer/physics/LaserRay.h"
#include "raytracer/physics/Magnitudes.h"
#include "raytracer/physics/LaserPropagation.h"
#include "raytracer/geometry/MeshFunction.h"

using namespace testing;
using namespace raytracer::physics;
using namespace raytracer::geometry;

class MeshFunctionMock : public MeshFunction {
public:
    double getValue(const Element &element) const override {
        return this->_value;
    }

    void setValue(const Element &element, double value) override {
        this->_value = value;
    }

    void addValue(const Element &element, double value) override {}

private:
    double _value{0};
};

class propagation_terminator : public Test {
public:
    void SetUp() override {
        laserRay.wavelength = Length{1315e-7};
        intersection.nextElement = &element;
    }

    LaserRay laserRay;
    MeshFunctionMock density;
    Element element{0, {}};
    Intersection intersection;

    StopAtCritical stopAtCritical{density};
    DontStop dontStop;
};

TEST_F(propagation_terminator, stop_at_critical_returns_true_if_density_at_element_is_bigger_than_density_and_vice_versa){
    auto critical = laserRay.getCriticalDensity();

    density.setValue(element, 1.1 * critical.asDouble);
    EXPECT_THAT(stopAtCritical(intersection, laserRay), true);
    density.setValue(element, 0.9 * critical.asDouble);
    ASSERT_THAT(stopAtCritical(intersection, laserRay), false);
}

TEST_F(propagation_terminator, dont_stop_returns_false_in_any_case){
    ASSERT_THAT(dontStop(intersection, laserRay), false);
}

class DensityInterfaceMeshFunctionMock : public MeshFunction {
public:
    double getValue(const Element &element) const override {
        return element.getId() == 0 ? 0 : 3.0 / 4.0 *  6.447e20;
    }

    void setValue(const Element &element, double value) override {}
    void addValue(const Element &element, double value) override {}
};

class MockGradientCalculator : public GradientCalculator {
public:
    Vector getGradient(const Intersection &intersection) const override {
        return {1,0};
    }
};

class MockCollisionalFrequencyCalculator : public CollisionalFrequencyCalculator {
public:
    Frequency
    getCollisionalFrequency(const Density &density, const Temperature &temperature, const Length &laserWavelength,
                            double ionization) const override {
        return Frequency{0};
    }
};

class propagation_direction : public Test {
public:
    void SetUp() override {
        laserRay.wavelength = Length{1315e-7};
        intersection.previousElement = &previousElement;
        intersection.nextElement = &nextElement;
        intersection.orientation = HalfLine{Point(0,0.1), Vector(1,sqrt(3)/3)};
        intersection.face = &faceD;
    }

    LaserRay laserRay;
    DensityInterfaceMeshFunctionMock density;
    MeshFunctionMock temperature;
    MeshFunctionMock ionization;
    Element previousElement{0, {}};

    Point pointA{0,0};
    Point pointB{1,0};
    Point pointC{1,1};
    Point pointD{0,1};
    Face faceA{0, {&pointA, &pointB}};
    Face faceB{1, {&pointB, &pointC}};
    Face faceC{2, {&pointC, &pointD}};
    Face faceD{3, {&pointD, &pointA}};

    Element nextElement{1, {&faceA, &faceB, &faceC, &faceD}};
    Intersection intersection;
    ContinueStraight continueStraight;
    MockGradientCalculator gradient;
    MockCollisionalFrequencyCalculator frequencyCalculator;
    SnellsLaw snellsLaw{density, temperature, ionization, gradient, frequencyCalculator};
};

TEST_F(propagation_direction, continue_straight_finds_the_correct_intersection){
    auto newIntersection = continueStraight(intersection, laserRay);

    EXPECT_THAT(newIntersection->orientation.point.x, DoubleEq(1));
    ASSERT_THAT(newIntersection->orientation.point.y, DoubleEq(0.1+sqrt(3)/3));
}

TEST_F(propagation_direction, snells_law_bends_the_ray_as_expected){
    auto newIntersection = snellsLaw(intersection, laserRay);

    EXPECT_THAT(newIntersection->orientation.point.x, DoubleNear(0, 1e-2));
    ASSERT_THAT(newIntersection->orientation.point.y, DoubleEq(1));
}