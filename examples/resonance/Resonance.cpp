#include <raytracer/geometry/Mesh.h>
#include <raytracer/physics/Laser.h>
#include <raytracer/geometry/MeshFunction.h>
#include <raytracer/physics/CollisionalFrequency.h>
#include <raytracer/physics/Gradient.h>
#include <raytracer/physics/Refraction.h>
#include <raytracer/physics/Propagation.h>
#include <raytracer/physics/Termination.h>
#include <raytracer/physics/Absorption.h>
#include "mfem.hpp"

void summarize_results(
        const std::map<const raytracer::AbsorptionModel *, raytracer::Energy> &modelsEnergies,
        raytracer::Energy initialEnergy,
        std::ostream &stream
) {
    double total = 0;
    for (auto const &modelEnergy : modelsEnergies) {
        const auto &model = modelEnergy.first;
        const auto &energy = modelEnergy.second;

        total += energy.asDouble;
        stream << model->getName() << ": " << energy.asDouble << " ... "
               << energy.asDouble / initialEnergy.asDouble * 100 << "%" << std::endl;
    }
    stream << "Total: " << total << " ... "
           << total / initialEnergy.asDouble * 100 << "%" << std::endl;
}

int main(int, char *[]) {
    using namespace raytracer;

    auto mfemMesh = std::make_unique<mfem::Mesh>("data/mesh.mesh", 1, 1, false);
    MfemMesh mesh(mfemMesh.get());
    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);

    mfem::GridFunction absorbedEnergyGridFunction(&l2FiniteElementSpace);
    absorbedEnergyGridFunction = 0;
    MfemMeshFunction absorbedEnergyMeshFunction(absorbedEnergyGridFunction, l2FiniteElementSpace);

    std::ifstream densityFile("data/density.txt");
    mfem::GridFunction densityGridFunction(mfemMesh.get(), densityFile);
    MfemMeshFunction densityMeshFunction(densityGridFunction, l2FiniteElementSpace);

    std::ifstream temperatureFile("data/temperature.txt");
    mfem::GridFunction temperatureGridFunction(mfemMesh.get(), temperatureFile);
    MfemMeshFunction temperatureMeshFunction(temperatureGridFunction, l2FiniteElementSpace);

    std::ifstream ionizationFile("data/ionization.txt");
    mfem::GridFunction ionizationGridFunction(mfemMesh.get(), ionizationFile);
    MfemMeshFunction ionizationMeshFunction(ionizationGridFunction, l2FiniteElementSpace);

    LinearInterpolation householder(mesh, densityMeshFunction, 30);
    householder.setGradient(false);

    SpitzerFrequency spitzerFrequency;

    Marker reflected;
    SnellsLaw snellsLaw(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            householder,
            spitzerFrequency,
            &reflected
    );
    Laser laser1(
            Length{800e-7},
            [](const Point &) { return Vector{-1, std::tan(1 / 180.0 * M_PI)}; },
            Gaussian(5e-4, 1, 0),
            Point(140 * 1e-4, 2e-4),
            Point(140 * 1e-4, 12e-4)
    );
    laser1.generateInitialRays(<#initializer#>, 1000);
    laser1.generateIntersections(mesh, snellsLaw, intersectStraight, DontStop());

    double initialEnergy = 0;
    for (const auto &ray : laser1.getRays()) {
        initialEnergy += ray.energy.asDouble;
    }

    Laser laser2(
            Length{800e-7},
            [](const Point &) { return Vector{-1, std::tan(2 / 180.0 * M_PI)}; },
            Gaussian(5e-4, 1, 0),
            Point(140 * 1e-4, 20e-4),
            Point(140 * 1e-4, 30e-4)
    );
    laser2.generateInitialRays(<#initializer#>, 1000);
    laser2.generateIntersections(mesh, snellsLaw, intersectStraight, DontStop());

    Laser laser3(
            Length{800e-7},
            [](const Point &) { return Vector{-1, std::tan(3 / 180.0 * M_PI)}; },
            Gaussian(5e-4, 1, 0),
            Point(140 * 1e-4, 40e-4),
            Point(140 * 1e-4, 50e-4)
    );
    laser3.generateInitialRays(<#initializer#>, 1000);
    laser3.generateIntersections(mesh, snellsLaw, intersectStraight, DontStop());

    Laser laser5(
            Length{800e-7},
            [](const Point &) { return Vector{-1, std::tan(5 / 180.0 * M_PI)}; },
            Gaussian(5e-4, 1, 0),
            Point(140 * 1e-4, 60e-4),
            Point(140 * 1e-4, 70e-4)
    );
    laser5.generateInitialRays(<#initializer#>, 1000);
    laser5.generateIntersections(mesh, snellsLaw, intersectStraight, DontStop());

    Resonance resonance(householder, reflected);
    AbsorptionController absorber;
    Bremsstrahlung bremsstrahlungModel(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            spitzerFrequency
    );
    absorber.addModel(&bremsstrahlungModel);
    absorber.addModel(&resonance);
    auto res1 = absorber.absorb(laser1, absorbedEnergyMeshFunction);
    auto res2 = absorber.absorb(laser2, absorbedEnergyMeshFunction);
    auto res3 = absorber.absorb(laser3, absorbedEnergyMeshFunction);
    auto res5 = absorber.absorb(laser5, absorbedEnergyMeshFunction);

    std::cout << "Angle 1" << std::endl;
    summarize_results(res1, Energy{initialEnergy}, std::cout);
    std::cout << std::endl;
    std::cout << "Angle 2" << std::endl;
    summarize_results(res2, Energy{initialEnergy}, std::cout);
    std::cout << std::endl;
    std::cout << "Angle 3" << std::endl;
    summarize_results(res3, Energy{initialEnergy}, std::cout);
    std::cout << std::endl;
    std::cout << "Angle 5" << std::endl;
    summarize_results(res5, Energy{initialEnergy}, std::cout);

    laser1.saveRaysToJson("data/rays1.json");
    laser2.saveRaysToJson("data/rays2.json");
    laser3.saveRaysToJson("data/rays3.json");
    laser5.saveRaysToJson("data/rays5.json");
    std::ofstream absorbedResult("data/absorbed_energy.txt");
    absorbedEnergyGridFunction.Save(absorbedResult);
}