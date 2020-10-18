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
#include <raytracer/physics/absorption.h>
#include "refraction.h"
#include "termination.h"

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

    LinearInterGrad gradient(mesh, densityMeshFunction, 0.2);
    gradient.setGradient();

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

    laser.generateInitialRays(<#initializer#>, 1000);
    laser.generateIntersections(mesh, snellsLaw, intersectStraight,DontStop());

    EnergyExchangeController absorber;
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