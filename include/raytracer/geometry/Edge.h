#ifndef RAYTRACER_EDGE_H
#define RAYTRACER_EDGE_H

#include "Point.h"
#include <cstdlib>

namespace raytracer {
    namespace geometry {
        /**
         * Class representing an Edge.
         * The edge is conclusively represented by two points.
         */
        class Edge {
        public:
            /**
             * Constriuct the edge given two points
             * @param startPoint
             * @param endPoint
             */
            Edge(const Point *startPoint, const Point *endPoint);

            /**
             * Default constructor.
             * When default constructed, both points are (0, 0).
             */
            Edge() = default;

            /**
             * Equality operator.
             * Two edges are equal if thei points are equal any any order.
             * @param anotherEdge
             * @return true if they almost equal
             */
            bool operator ==(const Edge& anotherEdge) const;

            /**
             * Edge start point
             */
            const Point* startPoint{};
            /**
             * Edge end point
             */
            const Point* endPoint{};

        private:
            size_t id{};

            bool pointsEqual(const Point& _startPoint, const Point& _endPoint) const;

            friend class Mesh;
        };
    }
}


#endif //RAYTRACER_EDGE_H
