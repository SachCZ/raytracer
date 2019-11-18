#include <stdexcept>
#include "Quadrilateral.h"

raytracer::geometry::Quadrilateral::Quadrilateral(std::vector<const raytracer::geometry::Point*> points) :
    points(std::move(points)){
    if (this->points.size() != 4) throw std::logic_error("Quadrilateral must be constructed from 3 points!");

    recalculateEdges();
}

void raytracer::geometry::Quadrilateral::recalculateEdges() {
    this->edges.emplace_back(Edge(this->points[0], this->points[1]));
    this->edges.emplace_back(Edge(this->points[1], this->points[2]));
    this->edges.emplace_back(Edge(this->points[2], this->points[3]));
    this->edges.emplace_back(Edge(this->points[3], this->points[0]));
}
