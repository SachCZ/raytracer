#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <memory>
#include <vector>

#include "Mesh.h"
#include "GeometryFunctions.h"


namespace raytracer {
    namespace geometry {
        /**
         * Structure representing a single intersection of HalfLine with a mesh.
         */
        struct Intersection {
            Vector direction{};

            PointOnFace pointOnFace{};

            /**Pointer to the next Element that the ray would go to from the Face.
             * Could be null if the ray just left the Mesh.*/
            const Element *nextElement{};
            /**Pointer to the previous Element that the ray actually came from.
             * could be null if the ray just entered the Mesh. */
            const Element *previousElement{};
        };

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
            template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
            std::vector<Intersection> findIntersections(
                    const Mesh &mesh,
                    DirectionFunction findDirection,
                    IntersectionFunction findIntersection,
                    StopCondition stopCondition) {

                std::vector<Intersection> result;

                std::unique_ptr<PointOnFace> initialPointOnFace = findClosestIntersection(
                        initialRay,
                        mesh.getBoundary()
                );

                if (!initialPointOnFace)
                    throw std::logic_error("No intersection found! Did you miss the target?");

                Intersection previousIntersection{};
                previousIntersection.nextElement = mesh.getFaceAdjacentElement(
                        initialPointOnFace->face,
                        initialRay.direction
                );
                if (!previousIntersection.nextElement) throw std::logic_error("Could not find next element at border!");
                previousIntersection.previousElement = nullptr;
                previousIntersection.pointOnFace = *initialPointOnFace;
                previousIntersection.direction = initialRay.direction;

                result.emplace_back(previousIntersection);

                while (!stopCondition(*(result.back().nextElement))){
                    auto nextElementForDirection = mesh.getFaceAdjacentElement( //NextElement
                            previousIntersection.pointOnFace.face,
                            previousIntersection.direction
                    );
                    if (!nextElementForDirection) break;
                    auto direction = findDirection(
                            previousIntersection.pointOnFace, //At which point
                            previousIntersection.direction, //Previous direction
                            *previousIntersection.nextElement, //Previous element
                            *nextElementForDirection
                    );

                    auto nextElementToGo = mesh.getFaceAdjacentElement(
                            previousIntersection.pointOnFace.face,
                            direction
                    );
                    if (!nextElementToGo) break;

                    PointOnFace nextPointOnFace = findIntersection(previousIntersection.pointOnFace, direction, *nextElementToGo);

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

        private:
            HalfLine initialRay;
        };
    }
}


#endif //RAYTRACER_RAY_H
