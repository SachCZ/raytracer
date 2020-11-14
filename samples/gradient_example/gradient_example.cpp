#include <raytracer.h>
#include <fenv.h>

int main(int, char *[]) {
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{1.0, 40}, SegmentedLine{1.0, 40});
    MfemL20Space space(mesh);
    MfemMeshFunction func(space, [](const Point& point){
        return std::atan(10*(point.x - 0.5)) + std::atan(10*(point.y - 0.5)) ;
    });
    std::ofstream output("householder.msgpack");
    auto gradient = calcHousGrad(mesh, func);
    output << gradient;
}