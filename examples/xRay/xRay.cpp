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
#include <random>

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

void saveDistributionToCSV(const std::string& filename, const raytracer::EnergyDistribution& energyDistribution){
    std::ofstream file(filename);
    for (const auto& energyAtPoint : energyDistribution.asArray){
        file << energyAtPoint.point.x << "," << energyAtPoint.point.y << "," << energyAtPoint.energy.asDouble << std::endl;
    }
}

struct FocusedBeamDirection {
    explicit FocusedBeamDirection(
            const raytracer::Point &focus) :
            focus(focus){}

    raytracer::Vector operator()(const raytracer::Point &point) {
        using namespace std;
        return focus - point;
    }

private:
    const raytracer::Point &focus;
};

struct XRayGain : public raytracer::AbsorptionModel {
    explicit XRayGain(const raytracer::MeshFunction& gain): gain(gain) {}

    raytracer::Energy getEnergyChange(const raytracer::Intersection &previousIntersection,
                                      const raytracer::Intersection &currentIntersection,
                                      const raytracer::Energy &currentEnergy,
                                      const raytracer::LaserRay &laserRay) const override {
        auto distance = (currentIntersection.pointOnFace.point - previousIntersection.pointOnFace.point).getNorm();
        auto element = currentIntersection.previousElement;
        auto gainCoeff = gain.getValue(*element);
        return raytracer::Energy{currentEnergy.asDouble * (1 - std::exp(gainCoeff*distance))};
    }

    std::string getName() const override {
        return "X-ray gain";
    }

private:
    const raytracer::MeshFunction& gain;
};

class CSVReader {
public:
    CSVReader(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string positionString, valueString;
            std::getline(ss, positionString,',');
            std::getline(ss, valueString, ',');
            auto position = std::stod(positionString);
            auto value = std::stod(valueString);
            positions.emplace_back(position);
            values.emplace_back(value);
        }
    }

    double atPositition(double currentPosition){
        for (int i = 0; i < positions.size(); i++){
            if (positions[i] > currentPosition * 1e4){
                return values[i];
            }
        }
        throw std::logic_error("No value found!");
    }

private:
    std::vector<double> positions;
    std::vector<double> values;
};


double densityFunction(const mfem::Vector &x) {
    static CSVReader densityReader("data/cell_density.csv");
    auto density = densityReader.atPositition(x(0));
    static CSVReader ionizationReader("data/cell_ionization.csv");
    auto ionization = ionizationReader.atPositition(x(0));
    auto massUnit = 1.6605e-24;
    auto A = 55.845;
    return ionization * density / A / massUnit;
}

double temperatureFunction(const mfem::Vector &x) {
    static CSVReader temperatureReader("data/cell_temperature.csv");
    return temperatureReader.atPositition(x(0));
}

double ionizationFunction(const mfem::Vector &x) {
    static CSVReader ionizationReader("data/cell_ionization.csv");
    return ionizationReader.atPositition(x(0));
}

double gainFunction(const mfem::Vector &x) {
    static CSVReader gainReader("data/gain.csv");
    return gainReader.atPositition(x(0));
}

int main(int, char *[]) {
    using namespace raytracer;

    auto mfemMesh = std::make_unique<mfem::Mesh>("data/mesh_long.mesh", 1, 1, false);
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

    std::ofstream densityFile("data/density.txt");
    densityGridFunction.Save(densityFile);

    mfem::GridFunction temperatureGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient temperatureFunctionCoefficient(temperatureFunction);
    temperatureGridFunction.ProjectCoefficient(temperatureFunctionCoefficient);
    MfemMeshFunction temperatureMeshFunction(temperatureGridFunction, l2FiniteElementSpace);

    std::ofstream temperatureFile("data/temperature.txt");
    temperatureGridFunction.Save(temperatureFile);

    mfem::GridFunction ionizationGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient ionizationFunctionCoefficient(ionizationFunction);
    ionizationGridFunction.ProjectCoefficient(ionizationFunctionCoefficient);
    MfemMeshFunction ionizationMeshFunction(ionizationGridFunction, l2FiniteElementSpace);

    std::ofstream ionizationFile("data/ionization.txt");
    ionizationGridFunction.Save(ionizationFile);

    mfem::GridFunction gainGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient gainFunctionCoefficient(gainFunction);
    gainGridFunction.ProjectCoefficient(gainFunctionCoefficient);
    MfemMeshFunction gainMeshFunction(gainGridFunction, l2FiniteElementSpace);

    std::ofstream gainFile("data/gain.txt");
    gainGridFunction.Save(gainFile);

    Householder householder(mesh, densityMeshFunction, 30);
    householder.update(false);

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

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> dist(-0.005 - 0.005, -0.005 + 0.005);
    Laser laser(
            Length{25.5e-7},
            [&dist, &engine](const Point&){return Vector{-0.005, 1};},
            //FocusedBeamDirection({-100*1e-4, 80*1e-4}),
            Gaussian(10e-4, 1, 0),
            Point(10*1e-4, -1*1e-4),
            Point(50*1e-4, -1*1e-4)
    );
    laser.generateRays(10000);
    saveDistributionToCSV("data/initial_distribution.csv", laser.getInitialEnergyDistribution());

    double sum = 0;
    for (const auto& laserRay : laser.getRays()){
        sum += laserRay.energy.asDouble;
    }

    std::cout<< sum << std::endl;

    laser.generateIntersections(mesh, snellsLaw, intersectStraight, DontStop());

    AbsorptionController absorber;

    Resonance resonance(householder, reflected);
    Bremsstrahlung bremsstrahlungModel(
            densityMeshFunction,
            temperatureMeshFunction,
            ionizationMeshFunction,
            spitzerFrequency
    );
    XRayGain gain(gainMeshFunction);
    absorber.addModel(&bremsstrahlungModel);
    absorber.addModel(&resonance);
    absorber.addModel(&gain);
    summarize_results(absorber.absorb(laser, absorbedEnergyMeshFunction), Energy{sum}, std::cout);

    laser.saveRaysToJson("data/rays.json");
    std::ofstream absorbedResult("data/absorbed_energy.txt");
    absorbedEnergyGridFunction.Save(absorbedResult);
}