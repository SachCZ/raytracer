#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/Laser.h>
#include <raytracer/physics/Magnitudes.h>
#include <mfem.hpp>

using namespace testing;
using namespace raytracer::physics;
using namespace raytracer::geometry;

class initialized_laser : public Test {

public:
    initialized_laser() {
        DiscreteLine side{};
        side.segmentCount = 15;
        side.length = 1;
        mfemMesh = constructRectangleMesh(side, side);
        mesh = std::make_unique<Mesh>(mfemMesh.get());
    }

    Laser laser{Length{1315e-7},
                [](const Point &point) { return Vector(1, 0); },
                [](double x) { return 1.2; },
                Point(-1, 0.1),
                Point(-1, 0.9)
    };
    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<Mesh> mesh;
};

TEST_F(initialized_laser, can_generateproper_rays) {
    laser.generateRays(51);
    auto rays = laser.getRays();
    EXPECT_THAT(rays, SizeIs(51));
    EXPECT_THAT(rays[25].startPoint.y, DoubleEq(0.5));
    EXPECT_THAT(rays[10].direction.x, DoubleEq(1));
    ASSERT_THAT(rays[5].energy.asDouble, DoubleEq(1.2 / 50*0.8));
}

TEST_F(initialized_laser, can_generate_intersections) {
    laser.generateRays(51);
    laser.generateIntersections(
            *mesh,
            [](const Intersection &intersection) {
                return findClosestIntersection(intersection.orientation, intersection.element->getFaces());
            },
            [](const Intersection &, const LaserRay& laserRay) { return false; }
    );

    ASSERT_THAT(laser.getRays()[17].intersections, SizeIs(16));
}