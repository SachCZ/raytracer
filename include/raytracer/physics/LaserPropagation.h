#ifndef RAYTRACER_LASERPROPAGATION_H
#define RAYTRACER_LASERPROPAGATION_H

#include "MeshFunction.h"
#include "LaserRay.h"
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
            explicit StopAtCritical(const geometry::MeshFunction &density);

            /** Returns true if the density at the laserRay next element to go to is greater than criticalDensity of
             * the laser ray
             *
             * @param intersection current laserRay intersection
             * @param laserRay
             * @return true if current density is grater than critical
             */
            bool operator()(const geometry::Intersection &intersection, const LaserRay &laserRay);

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
            bool operator()(const geometry::Intersection &, const LaserRay &);
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
                    const LaserRay &);
        };


        /**
         * Functor that given an intersection finds next intersection base on the Snells's law.
         */
        struct SnellsLaw {
            explicit SnellsLaw(const geometry::MeshFunction &density, const GradientCalculator &gradientCalculator);

            /**
             * Tries to find another intersection based on Snells law, density (provides an index of rarefaction)
             * and gradient (provides edge direction). Could throw error no intersection found!
             * @param intersection
             * @param laserRay
             * @return found intersection or throw
             */
            std::unique_ptr<geometry::Intersection>
            operator()(const geometry::Intersection &intersection, const LaserRay &laserRay);

        private:
            const geometry::MeshFunction &density;
            const GradientCalculator &gradientCalculator;

            geometry::Vector getDirection(const geometry::Intersection &intersection, const LaserRay &laserRay);
        };
    }
}


#endif //RAYTRACER_LASERPROPAGATION_H
