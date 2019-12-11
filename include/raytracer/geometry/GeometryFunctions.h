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
            /**Orientation is represented by HalfLine. The origin of the HalfLine is the intersection point.
             * The direction of the HalfLine is the direction the ray had when it intersected the Mesh.*/
            HalfLine orientation{};
            /**Pointer to the Face that was intersected by the ray.*/
            const Face* face{};
            /**Pointer to the next Element that the ray would go to from the Face.
             * Could be null if the ray just left the Mesh.*/
            const Element* nextElement{};
            /**Pointer to the previous Element that the ray actually came from.
             * could be null if the ray just entered the Mesh. */
            const Element* previousElement{};
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
