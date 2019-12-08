#ifndef RAYTRACER_GEOMETRY_FUNCTIONS_H
#define RAYTRACER_GEOMETRY_FUNCTIONS_H

#include <memory>

#include "Point.h"
#include "Vector.h"
#include "Face.h"
#include "Constants.h"
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
         * Structure representing a single intersection of HalfLine with a mesh.
         */
        struct Intersection {
            HalfLine orientation{};
            const Face* face{};
            const Element* element{};
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
        std::unique_ptr<Intersection> findIntersection(const HalfLine &halfLine, const Face *face);

        /**
         * For a given set of faces find all its intersections with a HalfLine and return the one that is closest to the
         * origin of the HalfLine.
         * @param halfLine
         * @param faces sequence of faces to be checked
         * @return an intersection structure pointer or nullptr if no intersection exists.
         */
        std::unique_ptr<Intersection> findClosestIntersection(const HalfLine &halfLine, const std::vector<Face *> &faces);
    }
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
