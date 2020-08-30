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
#include <raytracer/physics/Absorption.h>
#include "Refraction.h"
#include "Termination.h"

double initialDensityFunction(const mfem::Vector &x) {
    double critical = 6.4471308354624448e+20;
    return critical * 1.5;
}

double initialTemperatureFunction(const mfem::Vector &) {
    return 0.03;
}

double initialIonizationFunction(const mfem::Vector &) {
    return 0;
}

TEST(tracing, throught_mesh_should_work_as_expected_for_dummy_mesh) {
    using namespace raytracer;
    using namespace testing;

    auto mfemMesh = std::make_unique<mfem::Mesh>("data/mesh.vtk", 1, 0);

    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);

    MfemMesh mesh(mfemMesh.get());

    mfem::GridFunction densityGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient densityFunctionCoefficient(initialDensityFunction);
    densityGridFunction.ProjectCoefficient(densityFunctionCoefficient);
    MfemMeshFunction densityMeshFunction(densityGridFunction, l2FiniteElementSpace);

    mfem::GridFunction temperatureGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient temperatureFunctionCoefficient(initialTemperatureFunction);
    temperatureGridFunction.ProjectCoefficient(temperatureFunctionCoefficient);
    MfemMeshFunction temperatureMeshFunction(temperatureGridFunction, l2FiniteElementSpace);

    mfem::GridFunction ionizationGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient ionizationFunctionCoefficient(initialIonizationFunction);
    ionizationGridFunction.ProjectCoefficient(ionizationFunctionCoefficient);
    MfemMeshFunction ionizationMeshFunction(ionizationGridFunction, l2FiniteElementSpace);

    mfem::GridFunction absorbedEnergyGridFunction(&l2FiniteElementSpace);
    absorbedEnergyGridFunction = 0;
    MfemMeshFunction absorbedEnergyMeshFunction(absorbedEnergyGridFunction, l2FiniteElementSpace);

    Householder gradient(mesh, densityMeshFunction, 0.2);
    gradient.update();

    SpitzerFrequency spitzerFrequencyCalculator;
    Marker reflectionMarker;
    SnellsLaw snellsLaw(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            gradient,
            spitzerFrequencyCalculator,
            &reflectionMarker
    );


    Laser laser(
            Length{1315e-7},
            [](const Point) { return Vector(1, 0.5); },
            Gaussian(0.1),
            Point(-1.1, 0.8),
            Point(-1.1, 0.3)
    );

    laser.generateRays(1000);
    laser.generateIntersections(mesh, snellsLaw, intersectStraight,DontStop());

    AbsorptionController absorber;
    Resonance resonance(gradient, reflectionMarker);
    Bremsstrahlung bremsstrahlungModel(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            spitzerFrequencyCalculator
    );
    absorber.addModel(&bremsstrahlungModel);
    absorber.addModel(&resonance);
    absorber.absorb(laser, absorbedEnergyMeshFunction);

    //Randomly assert the number of intersections
    auto rays = laser.getRays();
    laser.saveRaysToJson("data/initial_rays.json");
    std::ofstream densityResult("data/initial_density.txt");
    densityGridFunction.Save(densityResult);

    std::ofstream energyResult("data/absorbed_energy.txt");
    absorbedEnergyGridFunction.Save(energyResult);
}