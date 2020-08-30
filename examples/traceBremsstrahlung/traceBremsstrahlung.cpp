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

double densityFunction(const mfem::Vector &x) {
    //return 1e26 * x(0) + 6.44713e+20;
    return x(0) > 0 && x(1) > 0 ? 1e28 : 1e6;
    //return 1e22  * (std::atan(x(0)*1e15)) - 1e22  * (std::atan(-5e-6*1e15));
}

double temperatureFunction(const mfem::Vector &x) {
    //return 2000;
    //return 2000 * (M_PI + std::atan(-x(0)*1e9));
    return x(0) > 0 && x(1) > 0 ? 2000 : 0.03;
}

double ionizationFunction(const mfem::Vector &x) {
    //return 22;
    return x(0) > 0 && x(1) > 0 ? 22 : 0;
}

int main(int, char *[]) {
    using namespace raytracer;

    auto mfemMesh = std::make_unique<mfem::Mesh>("data/mesh.vtk", 1, 0);
    MfemMesh mesh(mfemMesh.get());
    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);

    mfem::H1_FECollection h1FiniteElementCollection(1, 2);
    mfem::FiniteElementSpace h1FiniteElementSpace(mfemMesh.get(), &h1FiniteElementCollection);

    mfem::GridFunction absorbedEnergyGridFunction(&l2FiniteElementSpace);
    absorbedEnergyGridFunction = 0;
    MfemMeshFunction absorbedEnergyMeshFunction(absorbedEnergyGridFunction, l2FiniteElementSpace);

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

    //H1Gradient h1Gradient(l2FiniteElementSpace, h1FiniteElementSpace);
    //h1Gradient.updateDensity(densityGridFunction);
    //LeastSquare leastSquareGradient(mesh, densityMeshFunction);
    Householder householder(mesh, densityMeshFunction, 1e-5);
    householder.update();

    //NormalGradient normalGradient(densityMeshFunction);
    SpitzerFrequency spitzerFrequency;

    SnellsLaw snellsLaw(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            householder,
            spitzerFrequency
    );

    Laser laser(
            Length{1315e-7},
            [](const Point) { return Vector(1, 0.1); },
            Gaussian(0.3e-5, 1, 0),
            Point(-1.1e-6, 0.5e-6),
            Point(-1.1e-6, -0.5e-6)
    );

    laser.generateRays(1000);
    laser.generateIntersections(mesh, snellsLaw, intersectStraight, DontStop());

    AbsorptionController absorber;
    Bremsstrahlung bremsstrahlungModel(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            spitzerFrequency
    );
    absorber.addModel(&bremsstrahlungModel);
    absorber.absorb(laser, absorbedEnergyMeshFunction);

    laser.saveRaysToJson("data/rays.json");
    std::ofstream absorbedResult("data/absorbed_energy.txt");
    absorbedEnergyGridFunction.Save(absorbedResult);
}