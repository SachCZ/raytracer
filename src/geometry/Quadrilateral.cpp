#include "Quadrilateral.h"

raytracer::geometry::Quadrilateral::Quadrilateral(std::vector<raytracer::geometry::Point> points) : points(std::move(points))
{
    if (this->points.size() != 4) throw std::logic_error("Triangle must be constructed from 3 points!");
}
