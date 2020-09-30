#include <raytracer/geometry/Mesh.h>
#include <raytracer/physics/Refraction.h>
#include <cmath>

double densityFunction(const mfem::Vector &x) {
    return x(0) > 1 && x(0) < 4 && x(1) > 1 && x(1) < 4 ? 1 : 0;
}

int main(int argc, char *argv[]) {
    using namespace raytracer;

    auto mfemMesh = std::make_unique<mfem::Mesh>("mesh.mfem", 1, 1, false);

    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::H1_FECollection h1FiniteElementCollection(1, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mfemMesh.get(), &l2FiniteElementCollection);
    mfem::FiniteElementSpace h1FiniteElementSpace(mfemMesh.get(), &h1FiniteElementCollection);

    mfem::GridFunction l2Density(&l2FiniteElementSpace);
    mfem::FunctionCoefficient densityFunctionCoefficient(densityFunction);
    l2Density.ProjectCoefficient(densityFunctionCoefficient);

    auto h1Density = projectL2toH1(l2Density, l2FiniteElementSpace, h1FiniteElementSpace);

    char vishost[] = "localhost";
    int visport = 19916;
    mfem::socketstream sol_sock(vishost, visport);
    mfem::socketstream sol_sock2(vishost, visport);
    sol_sock.precision(8);
    sol_sock2.precision(8);
    sol_sock << "solution\n" << *mfemMesh << l2Density << std::flush;
    sol_sock2 << "solution\n" << *mfemMesh << h1Density << std::flush;

    return 0;
}
