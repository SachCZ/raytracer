#ifndef RAYTRACER_GEOMETRY_FUNCTIONS_H
#define RAYTRACER_GEOMETRY_FUNCTIONS_H


#include "mesh.h"

namespace raytracer {

    /**
     * Intersection of a ray with mesh
     */
    struct Intersection {
        /**
         * Direction of the intersecting ray.
         */
        Vector direction{};

        /**
         * PointOnFace where the Ray intersected a Mesh Face.
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

    /** Sequence of intersections */
    using Intersections = std::vector<Intersection>;
    /** Unique pointer to PointOnFace */
    using PointOnFacePtr = std::unique_ptr<PointOnFace>;
    /** Sequence of sequences of intersections */
    using IntersectionSet = std::vector<Intersections>;

    /**
     * Given a set of face find the closest intersection of the ray with on of the faces or return nullptr
     * @param ray
     * @param faces
     * @return
     */
    PointOnFacePtr findClosestIntersectionPoint(const Ray &ray, const std::vector<Face *> &faces);

    /**
     * Find all intersections by stepping through a mesh using the functions given.
     * @tparam DirectionFunction Vector(const PointOnFace &pointOnFace,
            const Vector &previousDirection,
            const Element &previousElement,
            const Element &nextElement)
     * @tparam IntersectionFunction PointOnFace(
            const PointOnFace &entryPointOnFace,
            const Vector &entryDirection,
            const Element &element)
     * @tparam StopCondition bool(const Element &element)
     * @param mesh
     * @param initialDirections rays incident on the mesh
     * @param findDirection function of type DirectionFunction
     * @param findIntersection function of type IntersectionFunction
     * @param stopCondition function of type StopCondition
     * @return Set of intersections
     */
    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    IntersectionSet findIntersections(const Mesh &mesh,
                                      const std::vector<Ray> &initialDirections,
                                      DirectionFunction findDirection,
                                      IntersectionFunction findIntersection,
                                      StopCondition stopCondition);

    //End of header, template garbage follows---------------------------------------------------------------------------




    namespace impl {
        template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
        Intersections findRayIntersections(
                const Mesh &mesh,
                const Ray &initialDirection,
                DirectionFunction findDirection,
                IntersectionFunction findIntersection,
                StopCondition stopCondition);
    }

    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    IntersectionSet findIntersections(
            const Mesh &mesh,
            const std::vector<Ray> &initialDirections,
            DirectionFunction findDirection,
            IntersectionFunction findIntersection,
            StopCondition stopCondition
    ) {
        IntersectionSet result;
        result.reserve(initialDirections.size());

        for (const auto &initialDirection : initialDirections) {
            result.emplace_back(impl::findRayIntersections(
                    mesh,
                    initialDirection,
                    findDirection,
                    findIntersection,
                    stopCondition
            ));
        }
        return result;
    }

    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    Intersections impl::findRayIntersections(
            const Mesh &mesh,
            const Ray &initialDirection,
            DirectionFunction findDirection,
            IntersectionFunction findIntersection,
            StopCondition stopCondition
    ) {
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
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
