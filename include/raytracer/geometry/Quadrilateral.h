#ifndef RAYTRACER_QUADRILATERAL_H
#define RAYTRACER_QUADRILATERAL_H

#include <vector>
#include <numeric>

#include "Point.h"
#include "Edge.h"

namespace raytracer {
    namespace geometry {

        class Mesh;

        /**
         * Class representing a quadrilateral
         */
        class Quadrilateral {
        public:
            /**
             * Construct the quadrilateral given a list of points.
             * If the list of points size is other than 4 the constructor throws a std::logic_error.
             * Order matters and it is assumed counterclockwise from left bottom corner.
             * @param points list of three points
             */
            explicit Quadrilateral(std::vector<const raytracer::geometry::Point*> points);

            /**
             * Get the center of the quad averiging
             * @return
             */
            Point getAveragePoint() const {
                auto sum = std::accumulate(this->points.begin(), this->points.end(), Vector(),
                        [](const Vector& vector, const Point* point){
                    auto result = *point + vector;
                    return Vector(result.x, result.y);
                });
                auto vector = 1.0/ this->points.size() * sum;
                return {vector.x, vector.y};
            }

            /**
             * Default constructor.
             * By default all the points are initialized to (0, 0).
             */
            Quadrilateral(): points{4}{}

            /**
             * Edges of the quadrilateral, initialized at construction.
             */
            std::vector<Edge> edges;

            /**
             * Get the quadrilateral points.
             * @return list of points
             */
        private:
            std::vector<const Point*> points;
            int id{0};

            void recalculateEdges();

            friend class Mesh;
            friend class MeshFunction;
        };
    }
}


#endif //RAYTRACER_QUADRILATERAL_H
