#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/LaserRay.h>
#include <raytracer/geometry/Vector.h>
#include <raytracer/geometry/Point.h>
#include <raytracer/physics/Magnitudes.h>

using namespace testing;
using namespace raytracer;

class laser_ray : public Test {
public:
    void SetUp() override {
        laserRay.startPoint = Point(-2, 0);
        laserRay.direction = Vector(1, 0);
        laserRay.energy = Energy{2.5};
        laserRay.wavelength = Length{1315e-7};
    }

    LaserRay laserRay;
};

TEST_F(laser_ray, is_properly_initialized) {
    EXPECT_THAT(laserRay.startPoint.x, DoubleEq(-2));
    EXPECT_THAT(laserRay.direction.x, DoubleEq(1));
    EXPECT_THAT(laserRay.energy.asDouble, DoubleEq(2.5));
    EXPECT_THAT(laserRay.wavelength.asDouble, DoubleEq(1315e-7));
}

TEST_F(laser_ray, critical_density_calculation_works){
    auto result = laserRay.getCriticalDensity();
    EXPECT_THAT(result.asDouble, DoubleNear(6.447e20, 1e17));
}

TEST_F(laser_ray, rerfactive_index_calculation_works){
    auto result = laserRay.getRefractiveIndex(Density{6.447e20}, Frequency{0});
    EXPECT_THAT(result, DoubleNear(0, 1e17));
}

TEST_F(laser_ray, permitivity_calculation_works){
    auto result = laserRay.getPermittivity(Density{6.447e20}, Frequency{0});
    EXPECT_THAT(result.real(), DoubleNear(0, 1e17));
    ASSERT_THAT(result.imag(), DoubleNear(0, 1e17));
}

TEST_F(laser_ray, inverse_bremsstrahlung_coefficient_calculation_works){
    auto result = laserRay.getInverseBremsstrahlungCoeff(Density{6.447e20}, Frequency{1e17});
    //TODO test this somehow
}

