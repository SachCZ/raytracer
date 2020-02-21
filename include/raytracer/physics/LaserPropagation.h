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

        /**
         * Functor for ray propagation that returns false in any case. Convenience struct to keep the call similar to
         * eg. StopAtCritical.
         */
        struct DontStop {
            /**
             * Just returns false.
             * @return false.
             */
            bool operator()(const geometry::Intersection &, const LaserRay &) {
                return false;
            }
        };

        /**
         * Functor that given an intersection finds next intersection in straight line through intersection.nextElement.
         */
        struct ContinueStraight {
            /**
             * Finds the closest intersection with intersection.nextElement
             * @param intersection
             * @return intersection unique pointer if intersection is found, else it returns nullptr
             */
            std::unique_ptr<geometry::Intersection> operator()(
                    const geometry::Intersection &intersection,
                    const LaserRay &) {
                return findClosestIntersection(
                        intersection.orientation,
                        intersection.nextElement->getFaces(),
                        intersection.face);
            }
        };


        struct SnellsLaw {
            explicit SnellsLaw(const geometry::MeshFunction &density, const GradientCalculator &gradientCalculator) :
                    density(density), gradientCalculator(gradientCalculator) {}

            std::unique_ptr<geometry::Intersection>
            operator()(const geometry::Intersection &intersection, const LaserRay &laserRay) {
                const auto previousElement = intersection.previousElement;
                const auto nextElement = intersection.nextElement;

                if (!previousElement) {
                    return findClosestIntersection(intersection.orientation, nextElement->getFaces(),
                                                   intersection.face);
                }

                auto orientation = geometry::HalfLine{
                        intersection.orientation.point,
                        getDirection(intersection, laserRay)
                };
                auto newIntersection = findClosestIntersection(
                        orientation,
                        nextElement->getFaces(),
                        intersection.face);
                if (!newIntersection) {
                    findClosestIntersection(
                            orientation,
                            previousElement->getFaces(),
                            intersection.face);
                    if (newIntersection){
                        newIntersection->nextElement = previousElement;
                    }
                }
                return newIntersection;
            }

        private:
            const geometry::MeshFunction &density;
            const GradientCalculator &gradientCalculator;

            geometry::Vector getDirection(const geometry::Intersection &intersection, const LaserRay &laserRay) {
                const auto rho1 = Density{density.getValue(*intersection.previousElement)};
                const auto rho2 = Density{density.getValue(*intersection.nextElement)};
                const double n1 = laserRay.getRefractiveIndex(rho1);
                const double n2 = laserRay.getRefractiveIndex(rho2);

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
