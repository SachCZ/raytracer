#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <raytracer/geometry/Point.h>
#include <raytracer/geometry/Quadrilateral.h>

using namespace testing;

class initialized_quad : public Test {
public:
    using Point = raytracer::geometry::Point;
    using Quad = raytracer::geometry::Quadrilateral;

    void SetUp() override {
        points.emplace_back(std::make_unique<Point>(0, 0));
        points.emplace_back(std::make_unique<Point>(1, 0));
        points.emplace_back(std::make_unique<Point>(1, 1));
        points.emplace_back(std::make_unique<Point>(0, 1));

        std::vector<const Point*> temp{points[0].get(), points[1].get(), points[2].get(), points[3].get()};
        quad = std::make_unique<Quad>(temp);
    }

public:

    std::vector<std::unique_ptr<Point>> points;
    std::unique_ptr<Quad> quad;
};

TEST_F(initialized_quad, has_three_specific_edges) {
    auto &edges = quad->edges;
    EXPECT_THAT(edges, SizeIs(4));

    EXPECT_THAT(edges[0].startPoint->x, DoubleEq(0));
    EXPECT_THAT(edges[0].startPoint->y, DoubleEq(0));
    EXPECT_THAT(edges[0].endPoint->x, DoubleEq(1));
    EXPECT_THAT(edges[0].endPoint->y, DoubleEq(0));

    EXPECT_THAT(edges[1].startPoint->x, DoubleEq(1));
    EXPECT_THAT(edges[1].startPoint->y, DoubleEq(0));
    EXPECT_THAT(edges[1].endPoint->x, DoubleEq(1));
    EXPECT_THAT(edges[1].endPoint->y, DoubleEq(1));

    EXPECT_THAT(edges[2].startPoint->x, DoubleEq(1));
    EXPECT_THAT(edges[2].startPoint->y, DoubleEq(1));
    EXPECT_THAT(edges[2].endPoint->x, DoubleEq(0));
    EXPECT_THAT(edges[2].endPoint->y, DoubleEq(1));
}



