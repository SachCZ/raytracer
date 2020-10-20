#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <physics.h>
#include <mfem.hpp>


using namespace testing;
using namespace raytracer;

class LaserTest : public Test {

public:
    Laser laser{Length{1315e-7},
                [](const Point &) { return Vector(1, 0); },
                [](double) { return 1.2; },
                Point(-1, 0.1),
                Point(-1, 0.9),
                51
    };
    MfemMesh mesh{SegmentedLine{1.0, 15}, SegmentedLine{1.0, 15}, mfem::Element::Type::QUADRILATERAL};
};

TEST_F(LaserTest, initial_directions_can_be_generated_from_laser) {
    auto initialDirection = generateInitialDirections(laser);
    EXPECT_THAT(initialDirection, SizeIs(51));
    EXPECT_THAT(initialDirection[25].origin.y, DoubleEq(0.5));
    EXPECT_THAT(initialDirection[10].direction.x, DoubleEq(1));
}


TEST_F(LaserTest, initial_energies_can_be_generated_from_laser) {
    auto initialEnergies = generateInitialEnergies(laser);
    ASSERT_THAT(initialEnergies[5].asDouble, DoubleEq(1.2 / 51 * 0.8));
}

TEST_F(LaserTest, can_generate_intersections) {
    auto intersections = findIntersections(
            mesh,
            generateInitialDirections(laser),
            ContinueStraight(),
            intersectStraight,
            dontStop
    );

    ASSERT_THAT(intersections[17], SizeIs(16));
}