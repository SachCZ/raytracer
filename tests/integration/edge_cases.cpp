#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer.h>
#include <cmath>
#include <matchers.h>

using namespace testing;

TEST(stuck_ray, is_detected_and_stopped) {
    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{0, 1, 4}, SegmentedLine{0, 1, 4});

    MfemMesh::Displacements displacements;
    for (const Point* point : mesh.getPoints()) {
        if (point->id == 7) {
            displacements.emplace_back(-0.24, 0.184);
        } else {
            displacements.emplace_back(0, 0);
        }
    }
    mesh.moveNodes(displacements);

    MfemL20Space space{mesh};

    MfemMeshFunction density(space, [](const Element& e){
        auto center = getElementCentroid(e);
        if (center.x > 0.5 || center.y > 0.5){
            return 1;
        } else {
            return 0;
        }
    });
    auto gradAtPoints = calcHousGrad(mesh, density);
    gradAtPoints[mesh.getPoints()[7]] = {0, -1};
    gradAtPoints[mesh.getPoints()[12]] = {0, 0};
    gradAtPoints[mesh.getPoints()[11]] = {0, 1};

    LinInterGrad gradient(gradAtPoints);
    MfemMeshFunction refractIndex(space, [](const Element& e){
        auto center = getElementCentroid(e);
        if (center.x > 0.5 || center.y > 0.5){
            return 0;
        } else {
            return 1;
        }
    });


    SnellsLaw snellsLaw(&refractIndex, nullptr, nullptr);
    snellsLaw.setGradCalc(gradient);
    std::vector<Ray> initDirs = {Ray{{-0.125, 0.375}, Vector{0.5, 0.125}}};

    auto intersectionSet = findIntersections(mesh, initDirs, snellsLaw, intersectStraight, dontStop);
    ASSERT_THAT(intersectionSet[0], SizeIs(18));
}

