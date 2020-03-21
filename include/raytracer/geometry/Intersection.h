#ifndef RAYTRACER_GEOMETRY_FUNCTIONS_H
#define RAYTRACER_GEOMETRY_FUNCTIONS_H

#include <vector>
#include <memory>

#include "Point.h"
#include "Face.h"
#include "Element.h"

namespace raytracer {

    /**
     * Structure representing a half line. It originates from a point and has a direction.
     */
    struct HalfLine {
        /**
         * Origin
         */
        Point origin;
        /**
         * Direction
         */
        Vector direction;
    };

    /**
     * Structure representing a point on a face. This struct is used as return type of findClosestIntersection().
     */
    struct PointOnFace {
        /**
         * The point.
         */
        Point point;
        /**
         * Pointer to the face the point is at.
         */
        const Face *face;
    };

    /**
     * Implementation namespace.
     * This namespace should be ignored by the end user/developer.
     */
    namespace impl {
        double getParamK(const HalfLine &halfLine, const std::vector<Point *> &points);

        double getParamT(const HalfLine &halfLine, const std::vector<Point *> &points);

        bool isIntersecting(double k, double t);
    }

    /**
     * Find an intersection of HalfLine with a Face. If there is no intersection a nullptr is returned.
     * @param halfLine that is expected to intersect the face
     * @param face to be intersected
     * @return PointOnFace or nullptr if no intersection exists.
     */
    std::unique_ptr<PointOnFace> findIntersection(const HalfLine &halfLine, const Face *face);

    /**
     * For a given set of faces use findIntersection() and return the one that is closest to the
     * origin of the HalfLine (euclidean norm).
     * You can optionally specify a Face wish to exclude from search.
     * @param halfLine that is expected to intersect one of the faces
     * @param faces sequence to be checked
     * @param omitFace faces address to omit when searching for intersection
     * @return PointOnFace or nullptr if no intersection exists
     */
    std::unique_ptr<PointOnFace> findClosestIntersection(
            const HalfLine &halfLine,
            const std::vector<Face *> &faces,
            const Face *omitFace = nullptr);

}

#endif //RAYTRACER_FREE_FUNCTIONS_H
