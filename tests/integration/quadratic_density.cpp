#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer.h>
#include <cmath>
#include <matchers.h>

TEST(single_ray, throught_mesh_should_work_as_expected_for_dummy_mesh) {
    using namespace raytracer;
    using namespace testing;

    MfemMesh mesh("data/mesh.vtk");

    mfem::L2_FECollection l2FiniteElementCollection(0, 2);
    mfem::FiniteElementSpace l2FiniteElementSpace(mesh.getMfemMesh(), &l2FiniteElementCollection);

    MfemMeshFunction density(l2FiniteElementSpace);
    density.setUsingFunction(mesh, [](Point point) {
        double critical = 6.4471308354624448e+20;
        return critical * (1 - point.x * point.x);
    });
    MfemMeshFunction temperature(l2FiniteElementSpace);
    temperature.setUsingFunction(mesh, [](Point) {
        return 200;
    });
    MfemMeshFunction ionization(l2FiniteElementSpace);
    ionization.setUsingFunction(mesh, [](Point) {
        return 1;
    });

    LinearInterGrad gradient(getHouseholderGradientAtPoints(mesh, density));
    SpitzerFrequency spitzerFrequencyCalculator;
    ColdPlasma coldPlasma;
    SnellsLaw snellsLaw(
            density,
            temperature,
            ionization,
            gradient,
            spitzerFrequencyCalculator,
            coldPlasma,
            Length{1315e-7}
    );

    auto intersectionSet = findIntersections(
            mesh,
            {Ray{{-1.1, 0.01}, Vector{1, 0.1}}},
            snellsLaw,
            intersectStraight,
            dontStop
    );
    auto lastIntersection = intersectionSet[0].back().pointOnFace.point;
    ASSERT_THAT(lastIntersection, IsSamePoint(Point{-1.0, 0.43880545104941487}));
}