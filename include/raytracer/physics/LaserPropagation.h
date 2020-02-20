#ifndef RAYTRACER_LASERPROPAGATION_H
#define RAYTRACER_LASERPROPAGATION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Vector.h"
#include "GradientCalculators.h"

namespace raytracer {
    namespace physics {
        /**
         * Functor for ray propagation termination based on critical density expected to used with laseRay
         * intersection finding procedure
         */
        struct StopAtCritical {
            /**
             * Constructor the functor using a density MeshFunction
             * @param density
             */
            explicit StopAtCritical(const geometry::MeshFunction &density) :
                    density(density) {}

            /** Returns true if the density at the laserRay next element to go to is greater than criticalDensity of
             * the laser ray
             *
             * @param intersection current laserRay intersection
             * @param laserRay
             * @return true if current density is grater than critical
             */
            bool operator()(const geometry::Intersection &intersection, const LaserRay &laserRay) {
                const auto element = *intersection.nextElement;

                auto currentDensity = density.getValue(element);
                auto criticalDensity = laserRay.getCriticalDensity();
                return currentDensity > criticalDensity.asDouble;
            }

        private:
            const geometry::MeshFunction &density;
        };

        struct DontStop {
            bool operator()(const geometry::Intersection&, const LaserRay&) {
                return false;
            }
        };


        std::unique_ptr<geometry::Intersection> continueStraight(const geometry::Intersection &intersection, const LaserRay &) {
            return findClosestIntersection(intersection.orientation, intersection.nextElement->getFaces(), intersection.face);
        }

        struct SnellsLaw {
            explicit SnellsLaw(const geometry::MeshFunction &density, const GradientCalculator &gradientCalculator) :
                    density(density), gradientCalculator(gradientCalculator) {}

            std::unique_ptr<geometry::Intersection> operator()(const geometry::Intersection &intersection, const LaserRay &laserRay) {
                const auto previousElement = intersection.previousElement;
                const auto nextElement = intersection.nextElement;

                if (!previousElement) {
                    return findClosestIntersection(intersection.orientation, nextElement->getFaces(), intersection.face);
                }

                auto newIntersection = findClosestIntersection(
                        intersection.orientation,
                        nextElement->getFaces(),
                        intersection.face);
                if (!newIntersection)
                    return newIntersection;
                newIntersection->orientation.direction = getDirection(intersection, laserRay.getCriticalDensity());
                return newIntersection;
            }

        private:
            const geometry::MeshFunction &density;
            const GradientCalculator &gradientCalculator;

            geometry::Vector getDirection(const geometry::Intersection &intersection, Density criticalDensity) {
                const double n1 = std::sqrt(1 - density.getValue(*intersection.previousElement) / criticalDensity.asDouble);
                const double n2 = std::sqrt(1 - density.getValue(*intersection.nextElement) / criticalDensity.asDouble);

                const auto gradient = gradientCalculator.getGradient(intersection);
                const auto &direction = intersection.orientation.direction;

                const auto l = 1 / direction.getNorm() * direction;
                auto n = 1 / gradient.getNorm() * gradient;
                auto c = (-1) * n * l;
                if (c < 0) {
                    c = -c;
                    n = (-1) * n;
                }
                const double r = n1 / n2;

                auto root = 1 - r * r * (1 - c * c);
                if (root > 0) {
                    return r * l + (r * c - std::sqrt(root)) * n;
                } else {
                    return l + 2 * c * n;
                }
            }
        };
    }
}


#endif //RAYTRACER_LASERPROPAGATION_H
