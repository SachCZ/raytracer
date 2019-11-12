#ifndef RAYTRACER_EDGE_H
#define RAYTRACER_EDGE_H

#include "Point.h"

namespace raytracer {
    namespace geometry {
        class Edge {
        public:
            Edge(Point startPoint, Point endPoint);
            Edge() = default;

            bool operator ==(const Edge& anotherEdge) const {
                return this->pointsEqual(anotherEdge.startPoint, anotherEdge.endPoint) ||
                this->pointsEqual(anotherEdge.endPoint, anotherEdge.startPoint);
            }

            Point startPoint;
            Point endPoint;

        private:
            size_t id;

            bool pointsEqual(const Point& _startPoint, const Point& _endPoint) const {
                return _startPoint == this->startPoint && _endPoint == this->endPoint;
            }

            friend class Mesh;
        };
    }
}


#endif //RAYTRACER_EDGE_H
