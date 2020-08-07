#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/physics/Propagation.h>
#include "raytracer/geometry/Ray.h"
#include "raytracer/geometry/Point.h"
#include "raytracer/geometry/Vector.h"
#include "raytracer/geometry/Mesh.h"

using namespace testing;
using namespace raytracer;

class initialized_ray : public Test {
public:
public:
    initialized_ray() {
        DiscreteLine sideA{10.0, 5};
        mfemMesh = std::move(constructMfemMesh(sideA, sideA));
        mesh = std::make_unique<MfemMesh>(mfemMesh.get());
    }

    std::unique_ptr <mfem::Mesh> mfemMesh;
    std::unique_ptr <MfemMesh> mesh;

    HalfLine halfLine{Point(-1, 4.5), Vector(1, 0)};
    Ray ray{halfLine};
};

TEST_F(initialized_ray, trace_through_steps_throught_mesh_according_to_find_intersection) {
    auto intersections = ray.findIntersections(
            *mesh,
            [](
                    const PointOnFace &,
                    const Vector &previousDirection,
                    const Element &,
                    const Element &
            ) {
                return previousDirection;
            },
            [](
                    const PointOnFace &pointOnFace,
                    const Vector &direction,
                    const Element &nextElement
            ) {
                return intersectStraight(pointOnFace, direction, nextElement);
            },
            [](const Element &) { return false; }
    );
    ASSERT_THAT(intersections, SizeIs(11));
}

TEST_F(initialized_ray, intersecting_can_deal_with_diagonal_case) {
    HalfLine diagonalHalfLine{Point(-1, 9), Vector(1, -1)};
    Ray diagonalRay{diagonalHalfLine};

    auto intersections = diagonalRay.findIntersections(
            *mesh,
            [](
                    const PointOnFace &,
                    const Vector &previousDirection,
                    const Element &,
                    const Element &
            ) {
                return previousDirection;
            },
            intersectStraight,
            [](const Element &) { return false; }
    );
    ASSERT_THAT(intersections, SizeIs(19));
}