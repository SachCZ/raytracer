#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer.h>

TEST(hous_grad, calculates_grad_at_points_exactly) {
    using namespace raytracer;

    MfemMesh mesh(SegmentedLine{1.0, 40}, SegmentedLine{1.0, 40});
    MfemL20Space space(mesh);
    MfemMeshFunction func(space, [](const Point& point){
        return std::atan(10*(point.x - 0.5)) + std::atan(10*(point.y - 0.5)) ;
    });

    auto gradient = calcHousGrad(mesh, func);
}