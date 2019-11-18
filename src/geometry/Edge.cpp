#include "Edge.h"

raytracer::geometry::Edge::Edge(const Point *startPoint, const Point *endPoint)
        : startPoint(startPoint), endPoint(endPoint) {}

bool raytracer::geometry::Edge::operator==(const raytracer::geometry::Edge &anotherEdge) const {
    return this->pointsEqual(*anotherEdge.startPoint, *anotherEdge.endPoint) ||
           this->pointsEqual(*anotherEdge.endPoint, *anotherEdge.startPoint);
}

bool raytracer::geometry::Edge::pointsEqual(const raytracer::geometry::Point &_startPoint,
                                            const raytracer::geometry::Point &_endPoint) const {
    return _startPoint == *this->startPoint && _endPoint == *this->endPoint;
}
