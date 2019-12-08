#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "raytracer/geometry/Ray.h"
#include "raytracer/geometry/Point.h"
#include "raytracer/geometry/Vector.h"
#include "raytracer/geometry/Mesh.h"

using namespace testing;
using namespace raytracer::geometry;

class initialized_ray : public Test {
public:
    DiscreteLine sideA{10.0 , 10};
    Mesh mesh{sideA, sideA};
    HalfLine halfLine{Point(-1, 4.5), Vector(1, 0)};
    Ray ray{halfLine};
};

TEST_F(initialized_ray, trace_through_steps_throught_mesh_according_to_find_intersection){
    auto intersections = ray.findIntersections(
            mesh,
            [](const Intersection& previousIntersection) -> std::unique_ptr<Intersection> {
                const auto element = previousIntersection.element;
                return findClosestIntersection(previousIntersection.orientation, element->getFaces());
            },
            [](const Intersection& previousIntersection){
                return false;
            });
    ASSERT_THAT(intersections, SizeIs(11));
}