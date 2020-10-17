#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <mfem.hpp>
#include <raytracer/geometry/mesh.h>
#include <raytracer/geometry/mesh_function.h>
#include <raytracer/physics/gradient.h>
#include <raytracer/physics/collisional_frequency.h>
#include <raytracer/physics/propagation.h>
#include <raytracer/physics/laser.h>
#include <raytracer/utility/numeric.h>
#include "refraction.h"
#include "termination.h"

double densityFunction(const mfem::Vector &x) {
    return 12.8e20 * x(0) + 12.8e20;
}

double temperatureFunction(const mfem::Vector &) {
    return 200;
}

double ionizationFunction(const mfem::Vector &) {
    return 0;
}

TEST(tracing, should_work_as_expected) {
    using namespace raytracer;
    using namespace testing;

    auto mfemMesh = std::make_unique<mfem::Mesh>("data/mesh.vtk", 1, 0);

    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);

    MfemMesh mesh(mfemMesh.get());

    mfem::GridFunction densityGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient densityFunctionCoefficient(densityFunction);
    densityGridFunction.ProjectCoefficient(densityFunctionCoefficient);
    MfemMeshFunction densityMeshFunction(densityGridFunction, l2FiniteElementSpace);

    mfem::GridFunction temperatureGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient temperatureFunctionCoefficient(temperatureFunction);
    temperatureGridFunction.ProjectCoefficient(temperatureFunctionCoefficient);
    MfemMeshFunction temperatureMeshFunction(temperatureGridFunction, l2FiniteElementSpace);

    mfem::GridFunction ionizationGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient ionizationFunctionCoefficient(ionizationFunction);
    ionizationGridFunction.ProjectCoefficient(ionizationFunctionCoefficient);
    MfemMeshFunction ionizationMeshFunction(ionizationGridFunction, l2FiniteElementSpace);

    ConstantGradient gradientCalculator(Vector(12.8e20, 0));

    SpitzerFrequency spitzerFrequencyCalculator;
    SnellsLaw snellsLaw(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            gradientCalculator,
            spitzerFrequencyCalculator
    );


    Laser laser(
            Length{1315e-7},
            [](const Point) { return Vector(1, 0.3); },
            Gaussian(0.1),
            Point(-1.1, 0.5),
            Point(-1.1, 0)
    );

    laser.generateInitialRays(<#initializer#>, 1000);
    laser.generateIntersections(mesh, snellsLaw, intersectStraight,DontStop());

    //Randomly assert the number of intersections
    auto rays = laser.getRays();
    laser.saveRaysToJson("data/rays.json");

    //EXPECT_THAT(rays[0].intersections, SizeIs(109));
    //EXPECT_THAT(rays[255].intersections, SizeIs(100));
    //EXPECT_THAT(rays[500].intersections, SizeIs(96));
    //EXPECT_THAT(rays[725].intersections, SizeIs(94));
    //ASSERT_THAT(rays[999].intersections, SizeIs(96));
}