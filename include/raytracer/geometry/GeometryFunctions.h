#ifndef RAYTRACER_GEOMETRY_FUNCTIONS_H
#define RAYTRACER_GEOMETRY_FUNCTIONS_H

#include <vector>
#include <memory>

#include "Point.h"
#include "Face.h"
#include "Element.h"

namespace raytracer {
    namespace geometry {

        /**
         * Structure representing a half line. It originates from a point and has a direction.
         */
        struct HalfLine {
            /**
             * Origin
             */
            Point point;
            /**
             * Direction
             */
            Vector direction;
        };

        /**
         * Structure representing a point on a face
         */
        struct PointOnFace {
            Point point;
            const Face* face;
        };

        /**
         * This namespace should be ignored by the end use
         */
        namespace impl {
            double getParamK(const HalfLine &halfLine, const std::vector<Point*> &points);

            double getParamT(const HalfLine &halfLine, const std::vector<Point*> &points);

            bool isIntersecting(double k, double t);
        }

        /**
         * Find an intersection of HalfLine with a face. If there is no intersection a nullptr is returned.
         * @param halfLine
         * @param face to be intersected
         * @return an intersection structure pointer or nullptr if no intersection exists.
         */
        std::unique_ptr<PointOnFace> findIntersection(const HalfLine &halfLine, const Face *face);

        /**
         * For a given set of faces find all its intersections with a HalfLine and return the one that is closest to the
         * origin of the HalfLine. You can optionally specify a Face* to face you wish to exclude from search.
         * @param halfLine
         * @param faces sequence of faces to be checked
         * @param omitFace faces to omit when searching for intersection
         * @return an intersection structure pointer or nullptr if no intersection exists.
         */
        std::unique_ptr<PointOnFace> findClosestIntersection(
                const HalfLine &halfLine,
                const std::vector<Face *> &faces,
                const Face* omitFace = nullptr);
    }
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
