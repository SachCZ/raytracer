#include <raytracer.h>

int main(int, char *[]) {
    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{1.0, 40}, SegmentedLine{1.0, 40});
    MfemL20Space space(mesh);
    MfemMeshFunction func(space, [](const Point& point){
        return std::atan(10*(point.x - 0.5)) + std::atan(10*(point.y - 0.5)) ;
    });
    std::ofstream meshOutput("dualMesh.mfem");
    writeDualMesh(meshOutput, mesh);
    std::ofstream output("householder.msgpack");
    auto gradient = calcHousGrad(mesh, func);
    output << gradient;
}