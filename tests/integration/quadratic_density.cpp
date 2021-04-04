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
    MfemMeshFunction refractIndex(space, [&density](const Element& e){
        return calcRefractIndex(density.getValue(e), Length{1315e-7}, 0);
    });
    SnellsLaw snellsLaw(gradient, refractIndex, nullptr, nullptr);
    std::vector<Ray> initDirs = {Ray{{-1.1, 0.01}, Vector{1, 0.1}}};

    auto intersectionSet = findIntersections(mesh, initDirs, snellsLaw, intersectStraight, dontStop);
    auto lastIntersection = intersectionSet[0].back().pointOnFace.point;
    ASSERT_THAT(lastIntersection, IsSamePoint(Point{-1.0, 0.40364564766135042}));
}