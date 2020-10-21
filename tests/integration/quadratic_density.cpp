#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer.h>
#include <cmath>
#include <matchers.h>

using namespace testing;

TEST(single_ray, throught_mesh_should_work_as_expected_for_dummy_mesh) {
    using namespace raytracer;

    MfemMesh mesh("data/mesh.vtk");
    MfemL20Space space{mesh};
    MfemMeshFunction density(space, [](Point point) {return 6.44e+20 * (1 - point.x * point.x);});

    LinInterGrad gradient(calcHousGrad(mesh, density));
    ColdPlasma coldPlasma{density, Length{1315e-7}};
    SnellsLaw snellsLaw(gradient, coldPlasma);
    std::vector<Ray> initDirs = {Ray{{-1.1, 0.01}, Vector{1, 0.1}}};

    auto intersectionSet = findIntersections(mesh, initDirs, snellsLaw, intersectStraight, dontStop);
    auto lastIntersection = intersectionSet[0].back().pointOnFace.point;
    ASSERT_THAT(lastIntersection, IsSamePoint(Point{-1.0, 0.41417462751621098}));
}