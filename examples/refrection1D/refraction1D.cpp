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

int main(int, char *[]) {
    using namespace raytracer;

    std::string meshFilename = "input/mesh.mesh";
    std::string densityFilename = "input/density.gf";
    std::string temperatureFilename = "input/temperature.gf";
    std::string ionizationFilename = "input/ionization.gf";
    Length laserWavelength{800e-7};
    Vector laserDirection{-1, std::tan(30.0 / 180.0 * M_PI)};
    double laserSpatialFWHM = 1e-4;
    Energy laserEnergy{1};
    Point laserStartPoint(140 * 1e-4, -50e-4);
    Point laserEndPoint(140 * 1e-4, -10 * 1e-4);
    int raysCount = 1000;
    bool estimateBremsstrahlung = true;
    bool estimateResonance = true;
    bool estimateGain = false;
    std::string gainFilename;
    if (estimateGain) {
        gainFilename = "input/gain.gf";
    }
    std::string raysOutputFilename = "output/rays.msgpack";
    std::string absorbedEnergyFilename = "output/absorbed_energy.gf";

    auto mfemMesh = std::make_unique<mfem::Mesh>(meshFilename.c_str(), 1, 1, false);
    MfemMesh mesh(mfemMesh.get());
    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);

    mfem::GridFunction absorbedEnergyGridFunction(&l2FiniteElementSpace);
    absorbedEnergyGridFunction = 0;
    MfemMeshFunction absorbedEnergyMeshFunction(absorbedEnergyGridFunction, l2FiniteElementSpace);

    std::ifstream densityFile(densityFilename);
    mfem::GridFunction densityGridFunction(mfemMesh.get(), densityFile);
    MfemMeshFunction densityMeshFunction(densityGridFunction, l2FiniteElementSpace);

    std::ifstream temperatureFile(temperatureFilename);
    mfem::GridFunction temperatureGridFunction(mfemMesh.get(), temperatureFile);
    MfemMeshFunction temperatureMeshFunction(temperatureGridFunction, l2FiniteElementSpace);

    std::ifstream ionizationFile(ionizationFilename);
    mfem::GridFunction ionizationGridFunction(mfemMesh.get(), ionizationFile);
    MfemMeshFunction ionizationMeshFunction(ionizationGridFunction, l2FiniteElementSpace);

    Laser laser(
            laserWavelength,
            [&laserDirection](const Point &) { return laserDirection; },
            Gaussian(laserSpatialFWHM, laserEnergy.asDouble, 0),
            laserStartPoint,
            laserEndPoint,
            raysCount
    );

    LinearInterpolation householderLinearInterpolation(getHouseholderGradientAtPoints(mesh, densityMeshFunction));

    SpitzerFrequency spitzerFrequency;
    ColdPlasma coldPlasma;

    Marker reflected;
    SnellsLaw snellsLaw(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            householderLinearInterpolation,
            spitzerFrequency,
            coldPlasma,
            laser.wavelength,
            &reflected
    );


    auto initialDirections = generateInitialDirections(laser);
    auto intersections = generateIntersections(
            mesh,
            initialDirections,
            snellsLaw,
            intersectStraight,
            DontStop()
    );

    AbsorptionController absorber;

    if (estimateBremsstrahlung){
        auto bremsstrahlungModel = std::make_unique<Bremsstrahlung>(
                densityMeshFunction,
                temperatureMeshFunction,
                ionizationMeshFunction,
                spitzerFrequency,
                coldPlasma,
                laser.wavelength
        );
        absorber.addModel(bremsstrahlungModel.get());
    }

    if (estimateResonance){
        ClassicCriticalDensity classicCriticalDensity;
        auto resonance = std::make_unique<Resonance>(householderLinearInterpolation, classicCriticalDensity, laser.wavelength, reflected);
        absorber.addModel(resonance.get());
    }

    if (estimateGain){
        std::ifstream gainFile(gainFilename);
        mfem::GridFunction gainGridFunction(mfemMesh.get(), gainFile);
        MfemMeshFunction gainMeshFunction(gainGridFunction, l2FiniteElementSpace);
        auto gain = std::make_unique<XRayGain>(gainMeshFunction);
        absorber.addModel(gain.get());
    }

    std::cout << stringifyAbsorptionSummary(
            absorber.absorb(intersections, generateInitialEnergies(laser), absorbedEnergyMeshFunction)
    );

    std::ofstream raysFile(raysOutputFilename);
    raysFile << stringifyRaysToMsgpack(intersections);
    std::ofstream absorbedResult(absorbedEnergyFilename);
    absorbedResult << absorbedEnergyMeshFunction;
}