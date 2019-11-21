#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <raytracer/geometry/MeshFunction.h>

using namespace testing;
using MeshFunction = raytracer::geometry::MeshFunction;
using Quadrilateral = raytracer::geometry::Quadrilateral;
using Point = raytracer::geometry::Point;

class mesh_function : public Test {
public:
    MeshFunction function;
    std::vector<Point> points{
            Point(0, 0),
            Point(0, 1),
            Point(1, 0),
            Point(1, 1)
    };
    Quadrilateral quadrilateral{{&points[0], &points[1], &points[2], &points[3]}};
};

TEST_F(mesh_function, value_can_be_set_using_set_all) {
    function.setAll({quadrilateral}, [](const Quadrilateral& quad){
        return 1.9;
    });
    auto values = function.getValues();
    EXPECT_THAT(values, SizeIs(1));
    ASSERT_THAT(values[0], DoubleEq(1.9));
}

TEST_F(mesh_function, can_be_subscripted_by_quad){
    function[quadrilateral] = 0.4;
    const auto value = function[quadrilateral];
    ASSERT_THAT(value, DoubleEq(0.4));
}