#ifndef RAYTRACER_EDGE_H
#define RAYTRACER_EDGE_H

#include "Point.h"

namespace raytracer {
    namespace geometry {
        class Edge {
        public:
            Edge(Point startPoint, Point endPoint);
            Edge() = default;

            Point startPoint;
            Point endPoint;
        };
    }
}


#endif //RAYTRACER_EDGE_H
