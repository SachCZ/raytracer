#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H

#include <vector>
#include "Point.h"
#include "Edge.h"

namespace raytracer {
    namespace geometry {

        class Mesh;

        /**
         * Class representing a triangle
         */
        class Triangle {
        public:
            /**
             * Construct the triangle given a list of points.
             * If the list of points size is other than three the constructor throws a std::logic_error
             * @param points list of three points
             */
            explicit Triangle(std::vector<raytracer::geometry::Point> points);

            /**
             * Default constructor.
             * By default all the points are initialized to (0, 0).
             */
            Triangle(): points{3}{}

            /**
             * Edges of the triangle, initialized at construction.
             */
            std::vector<Edge> edges;

            /**
             * Get the three triangle points.
             * @return list of points
             */
            const std::vector<Point>& getPoints() const;
        private:
            std::vector<Point> points;
            int id{0};

            void recalculateEdges();

            friend class Mesh;
        };
    }
}


#endif //RAYTRACER_TRIANGLE_H
