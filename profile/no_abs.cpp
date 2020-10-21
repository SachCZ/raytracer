#include <raytracer.h>
#include <chrono>

int main(int, char* []){
    using namespace raytracer;
    using timePoint = std::chrono::steady_clock::time_point;
    using namespace std::chrono;

    MfemMesh mesh("data/mesh.vtk");
    MfemL20Space space{mesh};
    MfemMeshFunction density(space, [](Point point) {return 6.44e+20 * (1 - point.x * point.x);});
    MfemMeshFunction temperature(space, [](Point) {return 200;});
    MfemMeshFunction ionization(space, [](Point) {return 1;});
    Length wavelength{1315e-7};

    LinInterGrad gradient(calcHousGrad(mesh, density));
    auto frequency = calcSpitzerFreq(density, temperature, ionization, wavelength);
    auto refractIndex = calcRefractiveIndex(density, wavelength, frequency.get());
    SnellsLaw snellsLaw(gradient, *refractIndex);

    int laserCount = 100000;
    std::vector<Ray> initDirs;
    initDirs.reserve(laserCount);
    for (int i = 0; i < laserCount; i++){
        initDirs.emplace_back(Ray{{-1.1, 0.01}, Vector{1, 0.1}});
    }
    timePoint begin = steady_clock::now();
    auto intersectionSet = findIntersections(mesh, initDirs, snellsLaw, intersectStraight, dontStop);
    timePoint end = steady_clock::now();
    int intersectionCount = 0;
    for (const auto& intersections : intersectionSet){
        intersectionCount += intersections.size();
    }
    std::cout << "Found " << intersectionCount << " intersections" << std::endl;
    std::cout << "Time elapsed = " << duration_cast<microseconds>(end - begin).count() * 1e-6 << " s" << std::endl;
}