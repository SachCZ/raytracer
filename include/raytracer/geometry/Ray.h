#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <memory>
#include <vector>

#include "Mesh.h"
#include "Intersection.h"

namespace raytracer {

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
        std::unique_ptr<PointOnFace> initialPointOnFace = findClosestIntersection(
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

}


#endif //RAYTRACER_RAY_H
