#include <raytracer.h>
#include <fstream>

int main(int, char *[]) {
    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{0.0, 1.0, 50}, SegmentedLine{0.0, 1.0, 10});
    MfemL20Space space{mesh};

    Length wavelength{1315e-7};

    MfemMeshFunction density(space, [&wavelength](Point point) {
        return calcCritDens(wavelength).asDouble * (1 - std::pow(point.x-1, 2));
    });
    MfemMeshFunction refractIndex(space, [&density, &wavelength](const Element& e){
        return calcRefractIndex(density.getValue(e), wavelength, 0);
    });

    LinInterGrad gradient(calcIntegralGrad(mesh, density));
    SnellsLaw snellsLaw(&refractIndex, nullptr, nullptr);
    snellsLaw.setGradCalc(gradient);
    auto intersectionSet = findIntersections(
            mesh,
            {Ray{{-0.1, 0.01}, Vector{1, 0.3}}},
            snellsLaw,
            intersectStraight,
            dontStop
    );
    
    std::ofstream trajectoryFile("trajectory.msgpack");
    trajectoryFile << stringifyRaysToMsgpack(intersectionSet);
    std::ofstream meshFile("mesh.mfem");
    meshFile << mesh;
    std::ofstream densityFile("density.gf");
    densityFile << density;
}