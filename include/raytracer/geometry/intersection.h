#ifndef RAYTRACER_GEOMETRY_FUNCTIONS_H
#define RAYTRACER_GEOMETRY_FUNCTIONS_H


#include "Mesh.h"

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
     * @ingroup api
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

        int id;
    };

    /**
     * Structure representing a single intersection of Ray with a Mesh.
     */
    struct Intersection {
        /**
         * Direction at which the Ray entered Intersection::nextElement.
         */
        Vector direction{};

        /**
         * PointOnFace where the Ray intersected the Face.
         */
        PointOnFace pointOnFace{};

        /**
         * Pointer to the next Element that the ray will go to from the Face.
         * Could be null if the ray just left the Mesh.
         */
        const Element *nextElement{};

        /**
         * Pointer to the previous Element that the ray actually came from.
         * Could be null if the ray just entered the Mesh.
         */
        const Element *previousElement{};
    };

    using Intersections = std::vector<Intersection>;

    using PointOnFacePtr = std::unique_ptr<PointOnFace>;

    using IntersectionSet = std::vector<Intersections>;


    /**
     * Implementation namespace.
     * This namespace should be ignored by the end user/developer.
     */
    namespace impl {
        double getParamK(const HalfLine &halfLine, const std::vector<Point *> &points);

        double getParamT(const HalfLine &halfLine, const std::vector<Point *> &points);

        bool isIntersecting(double k, double t, bool includePoint);

        PointOnFacePtr getClosest(std::vector<PointOnFacePtr> &intersections, const Point &point);
    }

    /**
     * Find an intersection of HalfLine with a Face. If there is no intersection a nullptr is returned.
     * @param halfLine that is expected to intersect the face
     * @param face to be intersected
     * @param includePoint whether to include the halfLine origin in the search
     * @return PointOnFace or nullptr if no intersection exists.
     */
    PointOnFacePtr findIntersectionPoint(const HalfLine &halfLine, const Face *face, bool includePoint = false);

    /**
     * For a given set of faces use findIntersection() and return the one that is closest to the
     * origin of the HalfLine (euclidean norm).
     * You can optionally specify a Face wish to exclude from search.
     * @param halfLine that is expected to intersect one of the faces
     * @param faces sequence to be checked
     * @param omitFace faces address to omit when searching for intersection
     * @return PointOnFace or nullptr if no intersection exists
     */
    PointOnFacePtr findClosestIntersectionPoint(const HalfLine &halfLine, const std::vector<Face *> &faces);

    /**
     * Find intersections finds all the Intersection with Mesh. There are multiple functional parameters to
     * ensure high variability of this method.
     *
     * @note
     * <b>This is the highly modular main method that does all the calculation.</b>
     *
     * @tparam DirectionFunction function type
     * @tparam IntersectionFunction function type
     * @tparam StopCondition function type
     * @param mesh to be intersected
     * @param findDirection function that decides new direction every time a Face is encountered.
     * The function must have the following form:
     * @code{.cpp}
        Vector findDirection()(
            const PointOnFace &pointOnFace,
            const Vector &previousDirection,
            const Element &previousElement,
            const Element &nextElement
        ) {
            //x, y = ...
            return Vector(x, y);
        }
     * @endcode
     * It is recommended that you copy and paste this to implement findDirection.
     * @param findIntersection function that finds the path of the Ray through given Element and returns
     * PointOnFace where the Ray escapes the Element.
     * The function mush have the following form:
     * @code{.cpp}
        PointOnFace findIntersection(
            const PointOnFace &entryPointOnFace,
            const Vector &entryDirection,
            const Element &element
        ) {
            //point, face = ...
            PointOnFace pointOnFace{};
            pointOnFace.point = point;
            pointOnFace.face = face;
            return pointOnFace;
        }
     * @endcode
     * It is recommended that you copy and paste this to implement findIntersection.
     * @param stopCondition function that returns true if the Ray should stop propagation.
     * The function mush have the following form:
     * @code{.cpp}
        bool stopCondition(
            const Element &
        ) {
            //shouldStop = ...
            return shouldStop;
        }
     * @endcode
     * It is recommended that you copy and paste this to implement stopCondition.
     * @return
     */
    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    Intersections findIntersections(
            const Mesh &mesh,
            const HalfLine& initialDirection,
            DirectionFunction findDirection,
            IntersectionFunction findIntersection,
            StopCondition stopCondition) {
        Intersections result;
        PointOnFacePtr initialPointOnFace = findClosestIntersectionPoint(
                initialDirection,
                mesh.getBoundary()
        );

        if (!initialPointOnFace)
            throw std::logic_error("No intersection found! Did you miss the target?");

        Intersection previousIntersection{};
        previousIntersection.nextElement = mesh.getFaceAdjacentElement(
                initialPointOnFace->face,
                initialDirection.direction
        );
        if (!previousIntersection.nextElement) throw std::logic_error("Could not find next element at border!");
        previousIntersection.previousElement = nullptr;
        previousIntersection.pointOnFace = *initialPointOnFace;
        previousIntersection.direction = initialDirection.direction;

        result.emplace_back(previousIntersection);

        while (result.back().nextElement && !stopCondition(*(result.back().nextElement))) {
            PointOnFace nextPointOnFace = findIntersection(
                    previousIntersection.pointOnFace,
                    previousIntersection.direction,
                    *previousIntersection.nextElement
            );

            auto nextElementForDirection = mesh.getFaceAdjacentElement( //NextElement
                    nextPointOnFace.face,
                    previousIntersection.direction
            );
            auto direction = previousIntersection.direction;
            if (nextElementForDirection) {
                direction = findDirection(
                        nextPointOnFace, //At which point
                        previousIntersection.direction, //Previous direction
                        *previousIntersection.nextElement, //Previous element
                        *nextElementForDirection
                );
            }

            auto nextElementToGo = mesh.getFaceAdjacentElement(
                    nextPointOnFace.face,
                    direction
            );

            Intersection intersection{};
            intersection.nextElement = nextElementToGo;
            intersection.previousElement = previousIntersection.nextElement;
            intersection.pointOnFace = nextPointOnFace;
            intersection.direction = direction;

            result.emplace_back(intersection);
            previousIntersection = intersection;
        }

        return result;
    }

    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    IntersectionSet generateIntersections(const Mesh &mesh,
                                          const std::vector<HalfLine> &initialDirections,
                                          DirectionFunction findDirection,
                                          IntersectionFunction findIntersection,
                                          StopCondition stopCondition) {
        IntersectionSet result;
        result.reserve(initialDirections.size());

        for (const auto &initialDirection : initialDirections) {
            result.emplace_back(findIntersections(
                    mesh,
                    initialDirection,
                    findDirection,
                    findIntersection,
                    stopCondition
            ));
        }
        return result;
    }
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
