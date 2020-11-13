#include <raytracer.h>
#include <fenv.h>

int main(int, char *[]) {
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{1.0, 10}, SegmentedLine{1.0, 10});
    MfemL20Space space(mesh);
    MfemMeshFunction func(space, [](const Point& point){
        return point.x;
    });
    std::ofstream output("householder.msgpack");
    auto gradient = calcHousGrad(mesh, func);
    output << gradient;
}