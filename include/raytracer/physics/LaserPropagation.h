#ifndef RAYTRACER_LASERPROPAGATION_H
#define RAYTRACER_LASERPROPAGATION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "GradientCalculators.h"
#include "CollisionalFrequencyCalculators.h"

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
            explicit StopAtCritical(const geometry::MeshFunction &density);

            /** Returns true if the density at the laserRay next element to go to is greater than criticalDensity of
             * the laser ray
             *
             * @param intersection current laserRay intersection
             * @param laserRay
             * @return true if current density is grater than critical
             */
            bool operator()(const geometry::Element &, const LaserRay &laserRay);

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
            bool operator()(const geometry::Element &, const LaserRay &);
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
            geometry::Vector operator()(
                    const geometry::PointOnFace &,
                    const geometry::Vector &previousDirection,
                    const geometry::Element &,
                    const geometry::Element &,
                    const LaserRay &
            ) {
                return previousDirection;
            }
        };


        /**
         * Functor that given an intersection finds next intersection base on the Snells's law.
         */
        struct SnellsLaw {
            /**
             * Construct the SnellsLaw using the density and temperature function reference. Provide gradientCalculator
             * to calculate the plane of rarefaction. Provide collisionalFrequencyCalculator to evaluate the index
             * of rarefaction.
             * Density and temperature values are expected to change.
             * @param density
             * @param temperature
             * @param gradientCalculator
             * @param collisionalFrequencyCalculator
             */
            explicit SnellsLaw(
                    const geometry::MeshFunction &density,
                    const geometry::MeshFunction &temperature,
                    const geometry::MeshFunction &ionization,
                    const GradientCalculator &gradientCalculator,
                    const CollisionalFrequencyCalculator &collisionalFrequencyCalculator
            );

            /**
             * Tries to find another intersection based on Snells law, density (provides an index of rarefaction)
             * and gradient (provides edge direction). Could throw error no intersection found!
             * @param intersection
             * @param laserRay
             * @return found intersection or throw
             */
            geometry::Vector operator()(
                    const geometry::PointOnFace &pointOnFace,
                    const geometry::Vector &previousDirection,
                    const geometry::Element &previousElement,
                    const geometry::Element &nextElement,
                    const LaserRay &laserRay
            );

        private:
            const geometry::MeshFunction &density;
            const geometry::MeshFunction &temperature;
            const geometry::MeshFunction &ionization;
            const GradientCalculator &gradientCalculator;
            const CollisionalFrequencyCalculator &collisionalFrequencyCalculator;
        };

        geometry::PointOnFace intersectStraight(
                const geometry::PointOnFace &pointOnFace,
                const geometry::Vector &direction,
                const geometry::Element &nextElement,
                const LaserRay &
        );
    }
}


#endif //RAYTRACER_LASERPROPAGATION_H
