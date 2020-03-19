#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <raytracer/geometry/Mesh.h>
#include <raytracer/physics/LaserPropagation.h>
#include <raytracer/physics/Laser.h>
#include <raytracer/physics/MathFunctions.h>
#include "raytracer/physics/GradientCalculators.h"

using namespace testing;
using namespace raytracer::physics;
using namespace raytracer::geometry;

class gradient_calculators : public Test {
    static double density(const mfem::Vector &x) {
        return 12 * x(0)  - 7 * x(1);
    }
public:
    void SetUp() override {
        mesh = std::make_unique<Mesh>(mfemMesh.get());
        l2FiniteElementSpace = std::make_unique<mfem::FiniteElementSpace>(mfemMesh.get(), &l2FiniteElementCollection);
        h1FiniteElementSpace = std::make_unique<mfem::FiniteElementSpace>(mfemMesh.get(), &h1FiniteElementCollection);

        h1GradientCalculator = std::make_unique<H1GradientCalculator>(*l2FiniteElementSpace, *h1FiniteElementSpace);

        densityGridFunction = std::make_unique<mfem::GridFunction>(l2FiniteElementSpace.get());
        mfem::FunctionCoefficient densityFunctionCoefficient(gradient_calculators::density);
        densityGridFunction->ProjectCoefficient(densityFunctionCoefficient);
    }

    ConstantGradientCalculator constantGradientCalculator{Vector(1.2, -0.3)};

    DiscreteLine side{100, 10};
    std::unique_ptr<mfem::Mesh> mfemMesh = constructRectangleMesh(side, side);

    mfem::L2_FECollection l2FiniteElementCollection{0, 2};
    mfem::H1_FECollection h1FiniteElementCollection{1, 2};
    std::unique_ptr<mfem::FiniteElementSpace> l2FiniteElementSpace;
    std::unique_ptr<mfem::FiniteElementSpace> h1FiniteElementSpace;
    std::unique_ptr<mfem::GridFunction> densityGridFunction;

    std::unique_ptr<H1GradientCalculator> h1GradientCalculator;
    std::unique_ptr<Mesh> mesh;
};

TEST_F(gradient_calculators, constant_gradient_calculator_returns_constant_vector_any_time){
    Intersection intersection{};
    auto result = constantGradientCalculator.getGradient(intersection);
    EXPECT_THAT(result.x, DoubleEq(1.2));
    EXPECT_THAT(result.y, DoubleEq(-0.3));
}

TEST_F(gradient_calculators, h1_returns_correct_result_for_linear_density){
    Laser laser(
            Length{1315e-7},
            [](const Point) { return Vector(1, 0.3); },
            Gaussian(0.1),
            Point(-1.1, 49),
            Point(-1.1, 51)
    );

    laser.generateRays(1);
    laser.generateIntersections(
            *mesh, ContinueStraight(),
            DontStop());
    auto ray = laser.getRays()[0];
    auto intersection = ray.intersections[5];
    h1GradientCalculator->updateDensity(*densityGridFunction);
    auto result = h1GradientCalculator->getGradient(intersection);

    //Pretty far
    EXPECT_THAT(result.x, DoubleNear(12, 1e-1));
    EXPECT_THAT(result.y, DoubleNear(-7, 1e-1));
}

TEST_F(gradient_calculators, h1_returns_correct_result_at_the_border){
    Laser laser(
            Length{1315e-7},
            [](const Point) { return Vector(1, 0.3); },
            Gaussian(0.1),
            Point(-1.1, 49),
            Point(-1.1, 51)
    );

    laser.generateRays(1);
    laser.generateIntersections(
            *mesh, ContinueStraight(),
            DontStop());
    auto ray = laser.getRays()[0];
    auto intersection = ray.intersections[0];
    h1GradientCalculator->updateDensity(*densityGridFunction);
    auto result = h1GradientCalculator->getGradient(intersection);

    //Gradient on border returns wrong result
    EXPECT_THAT(result.x, DoubleNear(12, 10));
    EXPECT_THAT(result.y, DoubleNear(-7, 10));
}