#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/Laser.h>
#include <raytracer/physics/Magnitudes.h>

using namespace testing;
using Laser = raytracer::physics::Laser;
using Point = raytracer::geometry::Point;
using Vector = raytracer::geometry::Vector;
using Length = raytracer::physics::Length;

class initialized_laser : public Test {

public:
    Laser laser{Length{1315e-7},
                [](const Point& point){return Vector(1.5, 0);},
                [](double x){return 1.2;},
                Point(0,0),
                Point(0, 1)
                };

};

TEST_F(initialized_laser, can_generateproper_rays) {
    auto rays = laser.generateRays(51);
    EXPECT_THAT(rays, SizeIs(51));
    EXPECT_THAT(rays[25].startPoint.y, DoubleEq(0.5));
    EXPECT_THAT(rays[10].direction.x, DoubleEq(1.5));
    ASSERT_THAT(rays[5].energy.asDouble, DoubleEq(1.2 / 50));
}