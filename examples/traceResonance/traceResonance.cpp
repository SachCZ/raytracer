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

using namespace raytracer;

const double scale = 5e11;

double densityFunction(const mfem::Vector &x) {
    //return 1e26 * x(0) + 6.44713e+20;
    //return x(0) > 0 && x(1) > 0 ? 1e22 : 1e6;
    return 6e19  * ((std::atan(x(0)*scale)) - (std::atan(-6e-6*scale)))*((std::atan(x(1)*scale)) - (std::atan(-6e-6*scale)));
}

double temperatureFunction(const mfem::Vector &x) {
    //return 2000;
    return 2000  * ((std::atan(x(0)*scale)) - (std::atan(-6e-6*scale)))*((std::atan(x(1)*scale)) - (std::atan(-6e-6*scale)));
    return x(0) > 0 && x(1) > 0 ? 2000 : 0.03;
}

double ionizationFunction(const mfem::Vector &x) {
    //return 22;
    return 22  * ((std::atan(x(0)*scale)) - (std::atan(-6e-6*scale)))*((std::atan(x(1)*scale)) - (std::atan(-6e-6*scale)));
    //return x(0) > 0 && x(1) > 0 > 0 ? 22 : 0;
}

int main(int, char *[]) {
    auto mfemMesh = std::make_unique<mfem::Mesh>("data/mesh.vtk", 1, 0);


    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::H1_FECollection h1FiniteElementCollection(1, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);
    mfem::FiniteElementSpace h1FiniteElementSpace(mfemMesh.get(), &h1FiniteElementCollection);

    mfem::GridFunction absorbedEnergyGridFunction(&l2FiniteElementSpace);
    absorbedEnergyGridFunction = 0;

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

    MfemMeshFunction absorbedEnergyMeshFunction(absorbedEnergyGridFunction, l2FiniteElementSpace);

    //LeastSquare leastSquareGradient(mesh, densityMeshFunction);
    //NormalGradient normalGradient(densityMeshFunction);
    LinearInterpolation householderGradient(mesh, densityMeshFunction, 1e-5);
    householderGradient.setGradient(true);
    SpitzerFrequency spitzerFrequency;

    Marker reflectedMarker;
    SnellsLaw snellsLaw(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            householderGradient,
            spitzerFrequency,
            &reflectedMarker
    );

    Laser laser(
            Length{1315e-7},
            [](const Point) { return Vector(1, 1); },
            Gaussian(0.3e-5),
            Point(-1.5e-5, -1e-5),
            Point(-0.5e-5, -1e-5)
    );

    laser.generateInitialRays(<#initializer#>, 20);
    laser.generateIntersections(mesh, snellsLaw, intersectStraight, DontStop());

    AbsorptionController absorber;
    Resonance resonance(householderGradient, reflectedMarker);
    absorber.addModel(&resonance);
    absorber.absorb(laser, absorbedEnergyMeshFunction);

    laser.saveRaysToJson("data/rays.json");
    std::ofstream absorbedResult("data/absorbed_energy.txt");
    absorbedEnergyGridFunction.Save(absorbedResult);
    std::ofstream densityResult("data/density.txt");
    densityGridFunction.Save(densityResult);
}