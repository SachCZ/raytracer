#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <string>
#include <raytracer/utility/JsonFormatter.h>

#include "Point.h"
#include "Edge.h"
#include "Quadrilateral.h"
#include "Mesh.h"

namespace raytracer {
    namespace geometry {
        /**
         * Simple data structure representing a single intersection
         */
        struct Intersection {
            Point point{};
            Edge edge{};
            Quadrilateral quadrilateral{};
        };

        /**
         * Simple data structure representing the current state of the ray
         * (info regarding place where it last intersected)
         */
        struct RayState {
            Vector currentDirection{};
            Intersection lastIntersection;
        };

        /**
         * Class representing a ray traveling through a mesh. It is first given by point and direction and
         * after raytracing by list of intersectios
         */
        class Ray {
        public:
            /**
             * Construct the ray using a start point and the ray direction.
             * @param startPoint
             * @param direction
             */
            Ray(Point startPoint, Vector direction);

            /**
             * A point from which the ray originates
             */
            Point startPoint;

            /**
             * The direction of the ray end.
             */
            Vector lastDirection; //TODO make this private

            /**
             * Find the closest intersection with given quads.
             * This method finds all the intersections with given quads and returns the one closest to the
             * last point of the ray
             * @param quads
             * @return single intersection
             */
            std::vector<Intersection>
            getClosestIntersection(const std::vector<Quadrilateral> &quads) const; //TODO make this private

            /**
             * Given a mesh find all the intersections based on getDirection.
             * Trace the ray through a mesh. Every time an intersection is encountered a new direction is chosen
             * based on the return value of getDirection function.
             *
             * getDirection is expected to be a function
             * taking Intersection lastIntersection as param and returning a vector
             * @tparam Function type
             * @param mesh
             * @param getDirection function returning a Vector - direction
             */
            template<typename DirectionFunction, typename StopCondition>
            void traceThrough(const Mesh &mesh, DirectionFunction getDirection, StopCondition stopCondition);

            /**
             * Get list of all the intersections
             * This wont have any value unless traceThrough was called.
             * @return list of intersections
             */
            const std::vector<Intersection> &getIntersections() const;

            /**
             * Saves the Ray to txt file.
             * Just the intersection points are saved simply by writing
             * each point coordinates on a new line separated by space
             * eg.: 1 2 etc.
             * @param filename without suffix (.txt will be appended)
             */
            void saveToTxt(const std::string &filename) const;

            /**
             * Save to file using JSON.
             * One JSON object is stored. It is just points: [...],
             * where [..] is a sequence of points eg. [[2, 3], [1, -1]]
             * @param filename withnout suffix (.json will be appended)
             */
            utility::json::Value getJsonValue() const;


        private:
            std::vector<Intersection> intersections;

            Point getLastPoint() const;

            std::vector<Intersection> findNext(const Quadrilateral& quad, const Mesh &mesh);

            std::vector<Intersection> getClosestPoint(const Point &point,
                                                      const std::vector<Intersection> &_intersections) const;

            Vector getNormal(const Vector &vector) const;

            double getParamK(const Edge &edge) const;

            double getParamT(const Edge &edge) const;

            bool isIntersecting(const Edge &edge) const;

            Point getIntersectionPoint(const Edge &edge) const;

            std::vector<Intersection> getQuadIntersections(const Quadrilateral &quad) const;

        };
    }
}

//Template definitions
template<typename DirectionFunction, typename StopCondition>
void raytracer::geometry::Ray::traceThrough(
        const raytracer::geometry::Mesh &mesh,
        DirectionFunction getDirection,
        StopCondition stopCondition) {
    auto previous = getClosestIntersection(mesh.boundary);
    RayState rayState;
    rayState.currentDirection = this->lastDirection;
    rayState.lastIntersection = previous[0];

    while (!previous.empty() && !stopCondition(rayState)) {
        intersections.emplace_back(previous[0]);
        rayState.currentDirection = this->lastDirection;
        rayState.lastIntersection = previous[0];
        this->lastDirection = getDirection(rayState);
        previous = this->findNext(previous[0].quadrilateral, mesh);
    }

    if (this->intersections.empty()) {
        throw std::logic_error("No intersections found! Did you miss the boundary?");
    }
}


#endif //RAYTRACER_RAY_H
