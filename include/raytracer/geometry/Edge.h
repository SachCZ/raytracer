#ifndef RAYTRACER_EDGE_H
#define RAYTRACER_EDGE_H

#include "Point.h"

namespace raytracer {
    namespace geometry {
        class Edge {
        public:
            Edge(Point startPoint, Point endPoint);
            Edge() = default;

            bool isEqual(const Edge& anotherEdge, double tolerance) const {
                return this->pointsEqual(anotherEdge.startPoint, anotherEdge.endPoint, tolerance) ||
                this->pointsEqual(anotherEdge.endPoint, anotherEdge.startPoint, tolerance);
            }

            Point startPoint;
            Point endPoint;

        private:
            size_t id;

            bool pointsEqual(const Point& _startPoint, const Point& _endPoint, double tolerance) const {
                return _startPoint.isEqual(this->startPoint, tolerance) &&
                _endPoint.isEqual(this->endPoint, tolerance);
            }

            friend class Mesh;
        };
    }
}


#endif //RAYTRACER_EDGE_H
