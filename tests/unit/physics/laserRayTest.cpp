#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/LaserRay.h>
#include <raytracer/geometry/Vector.h>
#include <raytracer/geometry/Point.h>
#include <raytracer/physics/Magnitudes.h>

using namespace testing;
using LaserRay = raytracer::physics::LaserRay;
using Vector = raytracer::geometry::Vector;
using Point = raytracer::geometry::Point;
using Energy = raytracer::physics::Energy;
using Length = raytracer::physics::Length;

class laser_ray : public Test {
public:
    void SetUp() override {
        laserRay.startPoint = Point(-2, 0);
        laserRay.direction = Vector(1, 0);
        laserRay.energy = Energy{2.5};
        laserRay.wavelength = Length{1e-9};
    }

    LaserRay laserRay;
};

TEST_F(laser_ray, is_properly_initialized) {
    EXPECT_THAT(laserRay.startPoint.x, DoubleEq(-2));
    EXPECT_THAT(laserRay.direction.x, DoubleEq(1));
    EXPECT_THAT(laserRay.energy.asDouble, DoubleEq(2.5));
    EXPECT_THAT(laserRay.wavelength.asDouble, DoubleEq(1e-9));
}