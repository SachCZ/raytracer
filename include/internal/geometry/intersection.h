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

    struct InterErrLog {
        std::size_t tooLong{0};
        std::size_t stuck{0};
        std::size_t notFound{0};
    };

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
                                      DirectionFunction&& findDirection,
                                      IntersectionFunction&& findIntersection,
                                      StopCondition&& stopCondition,
                                      InterErrLog *errLog = nullptr
    );

    //End of header, template garbage follows---------------------------------------------------------------------------




    namespace impl {
        template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
        Intersections findRayIntersections(
                const Mesh &mesh,
                const Ray &initialDirection,
                DirectionFunction&& findDirection,
                IntersectionFunction&& findIntersection,
                StopCondition&& stopCondition,
                InterErrLog *errLog = nullptr
        );
    }


    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    IntersectionSet findIntersections(
            const Mesh &mesh,
            const std::vector<Ray> &initialDirections,
            DirectionFunction&& findDirection,
            IntersectionFunction&& findIntersection,
            StopCondition&& stopCondition,
            InterErrLog *errLog
    ) {
        IntersectionSet result;
        result.reserve(initialDirections.size());

        for (const auto &initialDirection : initialDirections) {
            result.emplace_back(impl::findRayIntersections(
                    mesh,
                    initialDirection,
                    std::forward<DirectionFunction>(findDirection),
                    std::forward<IntersectionFunction>(findIntersection),
                    std::forward<StopCondition>(stopCondition),
                    errLog
            ));
        }
        return result;
    }

    template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
    Intersections impl::findRayIntersections(
            const Mesh &mesh,
            const Ray &initialDirection,
            DirectionFunction&& findDirection,
            IntersectionFunction&& findIntersection,
            StopCondition&& stopCondition,
            InterErrLog *errLog
    ) {
        Intersections result;
        PointOnFacePtr initialPointOnFace = findClosestIntersectionPoint(
                initialDirection,
                mesh.getBoundary()
        );

        if (!initialPointOnFace)
            throw std::logic_error("No intersection found! Did you miss the target?");

        Intersection previousIntersection{};
        previousIntersection.nextElement = mesh.getFaceDirAdjElement(
                initialPointOnFace->face,
                initialDirection.direction
        );
        if (!previousIntersection.nextElement) throw std::logic_error("Could not find next element at border!");
        previousIntersection.previousElement = nullptr;
        previousIntersection.pointOnFace = *initialPointOnFace;
        previousIntersection.direction = findDirection(
                *initialPointOnFace,
                initialDirection.direction,
                nullptr,
                previousIntersection.nextElement
        );


        result.emplace_back(previousIntersection);
        if (previousIntersection.direction * initialDirection.direction < 0) {
            return result;
        }

        while (result.back().nextElement && !stopCondition(*(result.back().nextElement))) {

            if (result.size() > 10000) {
                if (errLog) {
                    errLog->tooLong++;
                }
                break;
            }
            PointOnFace nextPointOnFace;
            try {
                nextPointOnFace = findIntersection(
                        previousIntersection.pointOnFace,
                        previousIntersection.direction,
                        *previousIntersection.nextElement
                );
            } catch (const std::logic_error &) {
                if (errLog) {
                    errLog->notFound++;
                }
                break;
            }

            auto nextElementForDirection = mesh.getFaceDirAdjElement( //NextElement
                    nextPointOnFace.face,
                    previousIntersection.direction
            );
            auto direction = findDirection(
                    nextPointOnFace, //At which point
                    previousIntersection.direction, //Previous direction
                    previousIntersection.nextElement, //Previous element
                    nextElementForDirection
            );

            auto nextElementToGo = mesh.getFaceDirAdjElement(
                    nextPointOnFace.face,
                    direction
            );

            Intersection intersection{};
            intersection.nextElement = nextElementToGo;
            intersection.previousElement = previousIntersection.nextElement;
            intersection.pointOnFace = nextPointOnFace;
            intersection.direction = direction;


            result.emplace_back(intersection);

            if (direction.x == 0 && direction.y == 0) break;

            previousIntersection = intersection;

            if (result.size() > 15) {
                const Element *possiblyStuck = intersection.previousElement;
                bool stuck = true;
                for (auto it = next(result.rbegin()); it != result.rbegin() + 10; it++) {
                    if (!(it->previousElement == it->nextElement && it->previousElement == possiblyStuck)) {
                        stuck = false;
                        break;
                    }
                }
                if (stuck) {
                    if (errLog) {
                        errLog->stuck++;
                    }
                    break;
                }
            }

        }

        return result;
    }
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
