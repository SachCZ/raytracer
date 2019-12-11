#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <memory>
#include <vector>

#include "Mesh.h"
#include "GeometryFunctions.h"


namespace raytracer {
    namespace geometry {

        /**
         * Class representing a ray propagating through mesh.
         * Here the convention is such that the ray is made up of segments.
         * In general it is not a line and must not be straight.
         */
        class Ray {
        public:
            /**
             * Construct it using a HalfLine (that is the initial orientation of the Ray)
             * @param initialRay
             */
            explicit Ray(const HalfLine &initialRay) :
                    initialRay(initialRay) {}

            /**
             * Given a mesh directionFunction and stopCondition find all intersections of the ray with a mesh.
             *
             * Parameter findInters must be a function with the following signature:
             * (const Intersection& previousIntersection, const Element& element) -> Vector.
             * The returned Vector is the next direction in which the ray should propagate.
             *
             * Parameter stopCondition must be a function with the following signature:
             * (const Intersection& previousIntersection, const Element& element) -> bool.
             * It should return true if the the ray propagation is to be stopped. Else it has no effect.
             *
             * @tparam IntersFunc Generic type used to represent a function
             * @tparam StopCondition Generic type used to represent a function
             * @param mesh to be intersected
             * @param findInters function with signature (const Intersection& previousIntersection, const Element& element) -> Vector
             * @param stopCondition function with signature (const Intersection& previousIntersection, const Element& element) -> bool.
             * @return sequence of found intersections
             */
            template<typename IntersFunc, typename StopCondition>
            std::vector<Intersection> findIntersections(
                    const Mesh &mesh,
                    IntersFunc findInters,
                    StopCondition stopCondition) {

                std::vector<Intersection> result;

                std::unique_ptr<Intersection> intersection = findClosestIntersection(initialRay, mesh.getBoundary());

                if (!intersection)
                    throw std::logic_error("No intersection found! Did you miss the target?");

                intersection->element = mesh.getAdjacentElement(
                        intersection->face,
                        intersection->orientation.direction);
                result.emplace_back(*intersection);


                while (intersection->element && !stopCondition(*intersection)) {
                    intersection = std::move(findInters(*intersection));
                    if (!intersection) break;
                    intersection->element = mesh.getAdjacentElement(
                            intersection->face,
                            intersection->orientation.direction);
                    result.emplace_back(*intersection);
                }
                return result;
            }

        private:
            HalfLine initialRay;
        };
    }
}


#endif //RAYTRACER_RAY_H
