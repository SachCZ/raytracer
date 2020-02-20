#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "raytracer/physics/LaserRay.h"
#include "raytracer/physics/Magnitudes.h"
#include "raytracer/physics/LaserPropagation.h"
#include "raytracer/geometry/MeshFunction.h"

using namespace testing;
using namespace raytracer::physics;
using namespace raytracer::geometry;

class DensityMeshFunctionMock : public MeshFunction {
public:
    double getValue(const Element &element) const override {
    }

    void setValue(const Element &element, double value) override {
    }

    void addValue(const Element &element, double value) override {}

private:
};

class propagators : public Test {
public:
    void SetUp() override {
        laserRay.wavelength = Length{1315e-7};
    }

    LaserRay laserRay;
    DensityMeshFunctionMock density;
    StopAtCritical stopAtCritical{density};
};

TEST_F(propagators, stop_at_critical_returns_true_if_density_at_element_is_bigger_than_density_and_vice_versa){
    auto critical = laserRay.getCriticalDensity();
    //EXPECT_THAT(stopAtCritical())
}