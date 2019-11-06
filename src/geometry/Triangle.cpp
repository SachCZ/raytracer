#include <stdexcept>
#include "geometry/Triangle.h"

raytracer::geometry::Triangle::Triangle(std::vector<raytracer::geometry::Point> points) : points(std::move(points)) {
    if (this->points.size() != 3) throw std::logic_error("Triangle must be constructed from 3 points!");

    recalculateEdges();
}

const std::vector<raytracer::geometry::Point> &raytracer::geometry::Triangle::getPoints() const {
    return this->points;
}

void raytracer::geometry::Triangle::recalculateEdges() {
    this->edges.emplace_back(Edge(this->points[0], this->points[1]));
    this->edges.emplace_back(Edge(this->points[1], this->points[2]));
    this->edges.emplace_back(Edge(this->points[2], this->points[0]));
}
