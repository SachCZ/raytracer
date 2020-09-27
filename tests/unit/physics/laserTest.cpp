#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/Laser.h>
#include <raytracer/physics/Magnitudes.h>
#include <mfem.hpp>
#include <raytracer/physics/Propagation.h>
#include <raytracer/physics/Refraction.h>
#include <raytracer/physics/Termination.h>

using namespace testing;
using namespace raytracer;

class initialized_laser : public Test {

public:
    initialized_laser() {
        DiscreteLine side{};
        side.segmentCount = 15;
        side.length = 1;
        mfemMesh = constructMfemMesh(side, side, mfem::Element::Type::QUADRILATERAL);
        mesh = std::make_unique<MfemMesh>(mfemMesh.get());
    }

    Laser laser{Length{1315e-7},
                [](const Point &point) { return Vector(1, 0); },
                [](double x) { return 1.2; },
                Point(-1, 0.1),
                Point(-1, 0.9)
    };
    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<MfemMesh> mesh;
};

TEST_F(initialized_laser, can_generateproper_rays) {
    laser.generateInitialRays(<#initializer#>, 51);
    auto rays = laser.getRays();
    EXPECT_THAT(rays, SizeIs(51));
    EXPECT_THAT(rays[25].startPoint.y, DoubleEq(0.5));
    EXPECT_THAT(rays[10].direction.x, DoubleEq(1));
    ASSERT_THAT(rays[5].energy.asDouble, DoubleEq(1.2 / 51 * 0.8));
}

TEST_F(initialized_laser, can_generate_intersections) {
    laser.generateInitialRays(<#initializer#>, 51);
    laser.generateIntersections(
            *mesh,
            ContinueStraight(),
            intersectStraight,
            DontStop()
    );

    ASSERT_THAT(laser.getRays()[17].intersections, SizeIs(16));
}