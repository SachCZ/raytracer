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
    return x(0) > 0 && x(1) > 0 ? 1e6 : 1e6;
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

struct FocusedBeamDirection {
    FocusedBeamDirection(
            const raytracer::Point &lineStart,
            const raytracer::Point &lineEnd,
            double focusDistance) :
            lineStart(lineStart), lineEnd(lineEnd), focusDistance(focusDistance){}

    raytracer::Vector operator()(const raytracer::Point &point) {
        using namespace std;
        raytracer::Point origin{(lineStart.x + lineEnd.x) / 2, (lineStart.y + lineEnd.y) / 2};
        auto y = (point - origin).getNorm();
        if ((point - lineStart).getNorm() > (point - lineEnd).getNorm()) { y = -y; }
        auto x = focusDistance;
        auto angle = angleBetween((lineEnd - lineStart).getNormal(), {1, 0});
        return rotate({x, y}, angle);
    }

private:
    const raytracer::Point &lineStart;
    const raytracer::Point &lineEnd;
    const double focusDistance;

    static double angleBetween(const raytracer::Vector &a, const raytracer::Vector &b) {
        auto aDir = 1 / a.getNorm() * a;
        auto bDir = 1 / b.getNorm() * b;
        return std::acos(aDir * bDir);
    }

    static raytracer::Vector rotate(const raytracer::Vector &a, double angle) {
        auto cs = std::cos(angle);
        auto sn = std::sin(angle);

        auto x = a.x * cs - a.y * sn;
        auto y = a.x * sn + a.y * cs;
        return {x, y};
    }
};

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

    Point a(-1.001e-6, -0.8e-6);
    Point b(-1.001e-6, 0.8e-6);
    Laser laser(
            Length{25.5e-7},
            FocusedBeamDirection(a, b, 1e-6),
            Gaussian(0.3e-5, 1, 0),
            a,
            b
    );

    laser.generateRays(100);
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

    std::ofstream densityResult("data/density.txt");
    densityGridFunction.Save(densityResult);
}