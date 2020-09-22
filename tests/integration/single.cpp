#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <mfem.hpp>
#include <raytracer/geometry/Mesh.h>
#include <raytracer/geometry/MeshFunction.h>
#include <raytracer/physics/Gradient.h>
#include <raytracer/physics/CollisionalFrequency.h>
#include <raytracer/physics/Propagation.h>
#include <raytracer/physics/Laser.h>
#include <raytracer/utility/FreeFunctions.h>
#include "Refraction.h"
#include "Termination.h"

double singleDensityFunction(const mfem::Vector &x) {
    double critical = 6.4471308354624448e+20;
    return critical * (1 - x(0) * x(0));
}

double singleTemperatureFunction(const mfem::Vector &) {
    return 200;
}

double singleIonizationFunction(const mfem::Vector &) {
    return 0;
}

TEST(single_ray, throught_mesh_should_work_as_expected_for_dummy_mesh) {
    using namespace raytracer;
    using namespace testing;

    auto mfemMesh = std::make_unique<mfem::Mesh>("data/mesh.vtk", 1, 0);

    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);

    MfemMesh mesh(mfemMesh.get());

    mfem::GridFunction densityGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient densityFunctionCoefficient(singleDensityFunction);
    densityGridFunction.ProjectCoefficient(densityFunctionCoefficient);
    MfemMeshFunction densityMeshFunction(densityGridFunction, l2FiniteElementSpace);

    mfem::GridFunction temperatureGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient temperatureFunctionCoefficient(singleTemperatureFunction);
    temperatureGridFunction.ProjectCoefficient(temperatureFunctionCoefficient);
    MfemMeshFunction temperatureMeshFunction(temperatureGridFunction, l2FiniteElementSpace);

    mfem::GridFunction ionizationGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient ionizationFunctionCoefficient(singleIonizationFunction);
    ionizationGridFunction.ProjectCoefficient(ionizationFunctionCoefficient);
    MfemMeshFunction ionizationMeshFunction(ionizationGridFunction, l2FiniteElementSpace);

    Householder gradient(mesh, densityMeshFunction, 0.2);
    gradient.update();

    SpitzerFrequency spitzerFrequencyCalculator;
    SnellsLaw snellsLaw(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            gradient,
            spitzerFrequencyCalculator
    );


    Laser laser1(
            Length{1315e-7},
            [](const Point) { return Vector(1, std::tan(0.1)); },
            Gaussian(0.1),
            Point(-1.1, 0.01),
            Point(-1.1, 0.01)
    );

    Laser laser2(
            Length{1315e-7},
            [](const Point) { return Vector(1, std::tan(0.2)); },
            Gaussian(0.1),
            Point(-1.1, 0.01),
            Point(-1.1, 0.01)
    );

    Laser laser3(
            Length{1315e-7},
            [](const Point) { return Vector(1, tan(0.25)); },
            Gaussian(0.1),
            Point(-1.1, 0.01),
            Point(-1.1, 0.01)
    );

    Laser laser4(
            Length{1315e-7},
            [](const Point) { return Vector(1, tan(0.5)); },
            Gaussian(0.1),
            Point(-1.1, 0.01),
            Point(-1.1, 0.01)
    );

    laser1.generateRays(1);
    laser1.generateIntersections(mesh, snellsLaw, intersectStraight,DontStop());
    laser2.generateRays(1);
    laser2.generateIntersections(mesh, snellsLaw, intersectStraight,DontStop());
    laser3.generateRays(1);
    laser3.generateIntersections(mesh, snellsLaw, intersectStraight,DontStop());
    laser4.generateRays(1);
    laser4.generateIntersections(mesh, snellsLaw, intersectStraight,DontStop());

    laser1.saveRaysToJson("data/ray1.json");
    laser2.saveRaysToJson("data/ray2.json");
    laser3.saveRaysToJson("data/ray3.json");
    laser4.saveRaysToJson("data/ray4.json");
    std::ofstream densityResult("data/density.txt");
    densityGridFunction.Save(densityResult);
}