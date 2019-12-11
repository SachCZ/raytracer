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
         * Structure representing a line. It has a point and a direction.
         */
        struct Line {
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
         * Structure representing a single intersection of Line with a mesh.
         */
        struct Intersection {
            /**Orientation is represented by Line. The origin of the Line is the intersection point.
             * The direction of the HalfLine is the direction the ray had when it intersected the Mesh.*/
            Line orientation{};
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
            double getParamK(const Line &halfLine, const std::vector<Point*> &points);

            double getParamT(const Line &halfLine, const std::vector<Point*> &points);

            bool isIntersecting(double k);
        }

        /**
         * Find an intersection of HalfLine with a face. If there is no intersection a nullptr is returned.
         * @param halfLine
         * @param face to be intersected
         * @return an intersection structure pointer or nullptr if no intersection exists.
         */
        std::unique_ptr<Intersection> findIntersection(const Line &halfLine, const Face *face);

        /**
         * For a given set of faces find all its intersections with a HalfLine and return the one that is closest to the
         * origin of the HalfLine. You can optionally specify a Face* to face you wish to exclude from search.
         * @param halfLine
         * @param faces sequence of faces to be checked
         * @param omitFace faces to omit when searching for intersection
         * @return an intersection structure pointer or nullptr if no intersection exists.
         */
        std::unique_ptr<Intersection> findClosestIntersection(
                const Line &halfLine,
                const std::vector<Face *> &faces,
                const Face* omitFace = nullptr);
    }
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
