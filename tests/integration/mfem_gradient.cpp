#include <gtest/gtest.h>
#include <raytracer.h>

TEST(mfem_grad, calculates_grad_at_points_not_exactly) {
    using namespace raytracer;

    SegmentedLine side{0.0, 1.0, 50};
    auto meshH = (side.end - side.start) / side.segmentCount;
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
            &gradientBoundaryValue,
            10.0,
            meshH
    );
}