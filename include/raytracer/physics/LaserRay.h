#ifndef RAYTRACER_LASER_RAY_H
#define RAYTRACER_LASER_RAY_H

#include <cmath>
#include <vector>

#include "Ray.h"
#include "Magnitudes.h"
#include "Point.h"
#include "Vector.h"
#include "Constants.h"
#include "GeometryFunctions.h"

namespace raytracer {
    namespace physics {

        /**
         * Structure representing a single LaserRay. It has associated energy but no width for now.
         */
        struct LaserRay {
            /** The point from which the LaserRay originates. */
            geometry::Point startPoint{};
            /** The initial direction of the LaserRay. */
            geometry::Vector direction{};
            /** Absolute energy carried by the LaserRay. */
            Energy energy{};
            /** Wavelength of the LaserRay in cm */
            Length wavelength{};

            /** The electron critical density of the LaserRay */
            Density getCriticalDensity() const;

            double getRefractiveIndex(const Density& density) const {
                return std::sqrt(1 - density.asDouble / this->getCriticalDensity().asDouble);
            }
            /** Wrapper around Ray::findIntersections.
             * It generates a ray based on the LaserRay properties and saves the result to LaserRay::intersections.
             *
             * @tparam IntersFunc function with signature (Intersection) -> std::unique_ptr<Intersection>
             * @tparam StopCondition function with signature (Intersection) -> bool
             * @param mesh to be intersected
             * @param findInters will be propagated to Ray::findIntersections as is.
             * @param stopCondition will be propagated to ray::findIntersections as is.
             */
            template<typename IntersFunc, typename StopCondition>
            void generateIntersections(
                    const geometry::Mesh &mesh,
                    IntersFunc findInters,
                    StopCondition stopCondition) {
                geometry::Ray ray(geometry::HalfLine{this->startPoint, this->direction});

                this->intersections = ray.findIntersections(mesh, findInters, stopCondition);
            }

            /** Sequence of all intersections with given Mesh.
             * This is empty if no generateIntersections was called.*/
            std::vector<geometry::Intersection> intersections;
        };
    }
}

#endif //RAYTRACER_LASER_RAY_H
