#include <raytracer.h>

int main(int, char *[]) {
    using namespace raytracer;

    SegmentedLine side{1, 50};
    //auto h = side.length / side.segmentCount;
    MfemMesh mesh(side, side);
    MfemL20Space space(mesh);

    MfemMeshFunction func(space, [](const Point &point) {
        return 3 * point.x + 4 * point.y;
    });

    mfem::Vector boundaryVal(2);
    boundaryVal[0] = 3;
    boundaryVal[1] = 4;
    mfem::VectorConstantCoefficient gradientBoundaryValue(boundaryVal);

    auto result = raytracer::mfemGradient(
            mesh,
            func,
            &gradientBoundaryValue
    );

    return 0;
}