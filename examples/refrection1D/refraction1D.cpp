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
#include "yaml-cpp/yaml.h"

raytracer::Vector parseVector(const YAML::Node& node){
    return {node["x"].as<double>(), node["y"].as<double>()};
}

raytracer::Point parsePoint(const YAML::Node& node){
    return {node["x"].as<double>(), node["y"].as<double>()};
}

int main(int, char *[]) {
    using namespace raytracer;

    YAML::Node config = YAML::LoadFile("input/config.yaml");

    std::string meshFilename = config["mesh_file"].as<std::string>();
    std::string densityFilename = config["density_file"].as<std::string>();
    std::string temperatureFilename = config["temperature_file"].as<std::string>();
    std::string ionizationFilename = config["ionization_file"].as<std::string>();
    Length laserWavelength{config["laser"]["wavelength"].as<double>()};
    auto laserDirection{parseVector(config["laser"]["direction"])};
    auto laserSpatialFWHM = config["laser"]["spatial_FWHM"].as<double>();
    Energy laserEnergy{config["laser"]["energy"].as<double>()};
    Point laserStartPoint(parsePoint(config["laser"]["start_point"]));
    Point laserEndPoint(parsePoint(config["laser"]["end_point"]));
    auto raysCount = config["laser"]["rays_count"].as<int>();
    auto estimateBremsstrahlung = config["estimate_bremsstrahlung"].as<bool>();
    bool estimateResonance = config["estimate_resonance"].as<bool>();
    bool estimateGain = config["estimate_gain"].as<bool>();
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

    std::unique_ptr<Bremsstrahlung> bremsstrahlungModel;
    if (estimateBremsstrahlung){
        bremsstrahlungModel = std::make_unique<Bremsstrahlung>(
                densityMeshFunction,
                temperatureMeshFunction,
                ionizationMeshFunction,
                spitzerFrequency,
                coldPlasma,
                laser.wavelength
        );
        absorber.addModel(bremsstrahlungModel.get());
    }

    std::unique_ptr<Resonance> resonance;
    ClassicCriticalDensity classicCriticalDensity;
    if (estimateResonance){
        resonance = std::make_unique<Resonance>(householderLinearInterpolation, classicCriticalDensity, laser.wavelength, reflected);
        absorber.addModel(resonance.get());
    }

    std::unique_ptr<XRayGain> gain;
    if (estimateGain){
        std::ifstream gainFile(config["gain_file"].as<std::string>());
        mfem::GridFunction gainGridFunction(mfemMesh.get(), gainFile);
        MfemMeshFunction gainMeshFunction(gainGridFunction, l2FiniteElementSpace);
        gain = std::make_unique<XRayGain>(gainMeshFunction);
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