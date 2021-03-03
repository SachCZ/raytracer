#include <raytracer.h>

int main(int, char *[]) {
    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{1, 50}, SegmentedLine{1, 50});
    MfemL20Space space(mesh);

    mfem::H1_FECollection h1FiniteElementCollection{1, 2};
    mfem::FiniteElementSpace h1FiniteElementSpace(mesh.getMfemMesh(), &h1FiniteElementCollection, 2);

    auto analytic = [](const Point& point){
        return 3*point.x + 4*point.y;
    };

    MfemMeshFunction func(space, analytic);

    auto* boundaryValue = new mfem::FunctionCoefficient([&](const mfem::Vector& point){
        return analytic({point[0], point[1]});
    });

    mfem::Vector boundaryVal(2);
    boundaryVal[0] = 3;
    boundaryVal[1] = 4;
    mfem::VectorConstantCoefficient gradientBoundaryValue(boundaryVal);

    auto result = raytracer::mfemGradient(
            *func.getGF(),
            space.getSpace(),
            h1FiniteElementSpace,
            mesh,
            *boundaryValue,
            gradientBoundaryValue
            );
}