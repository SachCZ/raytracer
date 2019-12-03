#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <memory>
#include <vector>

#include "Mesh.h"
#include "Functions.h"


namespace raytracer {
    namespace geometry {
        class Ray {
        public:
            explicit Ray(const HalfLine &initialRay) :
                    initialRay(initialRay) {}

            template<typename IntersFunc, typename StopCondition>
            std::vector<Intersection> findIntersections(
                    const Mesh &mesh,
                    IntersFunc findInters,
                    StopCondition stopCondition) {

                std::vector<Intersection> result;

                std::unique_ptr<Intersection> intersection = findClosestIntersection(initialRay, mesh.getBoundary());
                std::unique_ptr<Element> nextElement;

                if (!intersection)
                    throw std::logic_error("No intersection found! Did you miss the target?");


                do {
                    result.emplace_back(*intersection);
                    nextElement = std::move(mesh.getAdjacentElement(
                            intersection->face,
                            intersection->orientation.direction));
                    if (!nextElement) {
                        break;
                    }
                    intersection = std::move(findInters(*intersection, *nextElement));
                } while (intersection && !stopCondition(*intersection, *nextElement));
                return result;
            }

        private:
            HalfLine initialRay;
        };
    }
}


#endif //RAYTRACER_RAY_H
