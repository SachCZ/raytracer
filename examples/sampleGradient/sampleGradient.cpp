#include <raytracer/geometry/Mesh.h>
#include <raytracer/physics/Laser.h>
#include <raytracer/physics/Refraction.h>
#include <raytracer/physics/Propagation.h>
#include <raytracer/physics/Termination.h>
#include <cmath>
#include <fstream>

double densityFunction(const mfem::Vector &x) {
    return std::sin(2*M_PI*x(0)) + std::sin(2*M_PI*x(1));
    //return std::atan(x(0) - 0.5);
    //return x(0) > 0.5 ? 1 : 0;
}


class GradientSampler {
public:
    explicit GradientSampler(uint gridSize, double sideXLength = 1, double sideYLength = 1):
    gridSize(gridSize), sideXLength(sideXLength), sideYLength(sideYLength) {
        using namespace raytracer;
        if (gridSize % 2 == 0) throw std::logic_error("gridSize must be odd");

        DiscreteLine sideX{};
        sideX.segmentCount = (gridSize + 1) / 2;
        sideX.length = sideXLength;
        DiscreteLine sideY{};
        sideY.length = sideYLength + 2 * sideYLength / gridSize;
        sideY.segmentCount = gridSize + 2;

        mfemMesh = constructMfemMesh(sideX, sideY);
        mesh = std::make_unique<MfemMesh>(mfemMesh.get());
    }

    mfem::Mesh* getMfemMesh(){
        return mfemMesh.get();
    }

    raytracer::Mesh* getMesh(){
        return mesh.get();
    }


    void printMesh(const std::string& filename){
        std::ofstream meshFile(filename + ".vtk");
        mfemMesh->PrintVTK(meshFile);
    }

    void printGridSize(const std::string& filename){
        std::ofstream sizeFile(filename + ".csv");
        auto segment = sideXLength / gridSize;
        auto halfSegment = segment / 2;
        auto oneAndHalfSegment = 3 * halfSegment;
        sizeFile << segment << "," << sideXLength - segment << "," << gridSize <<std::endl;
        sizeFile << oneAndHalfSegment << "," << sideYLength + halfSegment << "," << gridSize;
    }

    void sample(std::string outputFilename, raytracer::Gradient* gradient, const std::string& axis){
        using namespace raytracer;


        auto halfSegment =  (sideYLength / gridSize) / 2.0;
        auto oneAndHalfSegment = 3 * halfSegment;
        Laser laser(
                Length{1},
                [](Point point) { return Vector(1, 0); },
                [](double x) { return 1; },
                Point(-0.1, oneAndHalfSegment),
                Point(-0.1, sideYLength + halfSegment)
        );
        laser.generateRays(gridSize);
        laser.generateIntersections(*mesh, ContinueStraight(), intersectStraight, DontStop());

        if (axis == "x"){
            outputFilename = outputFilename + "_x.csv";
        } else if (axis == "y"){
            outputFilename = outputFilename + "_y.csv";
        }

        std::ofstream file(outputFilename);
        auto rays = laser.getRays();
        for (const auto &ray : rays) {
            std::string delimiter;
            for (const auto &intersection : ray.intersections) {
                auto point = intersection.pointOnFace.point;
                if (!intersection.previousElement) continue;
                if (!intersection.nextElement) continue;
                auto gradientValue = gradient->get(
                        intersection.pointOnFace,
                        *intersection.previousElement,
                        *intersection.nextElement
                );
                if (axis == "x"){
                    file << delimiter << gradientValue.x;
                } else if (axis == "y"){
                    file << delimiter << gradientValue.y;
                }
                delimiter = ",";
            }
            file << std::endl;
        }

    }

private:
    uint gridSize;
    double sideXLength, sideYLength;
    std::unique_ptr<mfem::Mesh> mfemMesh;
    std::unique_ptr<raytracer::Mesh> mesh;
};





int main(int argc, char *argv[]) {
    using namespace raytracer;

    GradientSampler gradientSampler(31);

    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::H1_FECollection h1FiniteElementCollection(1, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(gradientSampler.getMfemMesh(), &l2FiniteElementCollection);
    mfem::FiniteElementSpace h1FiniteElementSpace(gradientSampler.getMfemMesh(), &h1FiniteElementCollection);

    mfem::GridFunction densityGridFunction(&l2FiniteElementSpace);
    mfem::FunctionCoefficient densityFunctionCoefficient(densityFunction);
    densityGridFunction.ProjectCoefficient(densityFunctionCoefficient);
    MfemMeshFunction densityMeshFunction(densityGridFunction, l2FiniteElementSpace);

    LeastSquare leastSquareGradient(*gradientSampler.getMesh(), densityMeshFunction);
    Householder householder(*gradientSampler.getMesh(), densityMeshFunction, 0.3);
    householder.update(false);
    H1Gradient h1Gradient(l2FiniteElementSpace, h1FiniteElementSpace);
    h1Gradient.updateDensity(densityGridFunction);

    gradientSampler.printGridSize("data/gridSize");
    gradientSampler.printMesh("data/mesh");
    gradientSampler.sample("data/h1_gradient", &h1Gradient, "x");
    gradientSampler.sample("data/h1_gradient", &h1Gradient, "y");
    gradientSampler.sample("data/least_square", &leastSquareGradient, "x");
    gradientSampler.sample("data/least_square", &leastSquareGradient, "y");
    gradientSampler.sample("data/householder", &householder, "x");
    gradientSampler.sample("data/householder", &householder, "y");

    return 0;
}
