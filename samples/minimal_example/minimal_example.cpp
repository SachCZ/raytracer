#include <raytracer.h>

int main(int, char *[]) {
    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{1.0, 50}, SegmentedLine{1.0, 10});
    MfemL20Space space{mesh};
    Length wavelength{1315e-7};
    MfemMeshFunction density(space, [&wavelength](Point point) {
        return calcCritDens(wavelength).asDouble * (1 - std::pow(point.x-1, 2));
    });

    LinInterGrad gradient(calcHousGrad(mesh, density));
    auto refractIndex = calcRefractiveIndex(density, wavelength);
    SnellsLaw snellsLaw(gradient, *refractIndex);
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
    std::cout << "Trajectory written to trajectory.msgpack, mesh writen to mesh.mfem" << std::endl;
}