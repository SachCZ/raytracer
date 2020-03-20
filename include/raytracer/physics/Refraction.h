#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Gradient.h"
#include "CollisionalFrequency.h"

namespace raytracer {
    namespace physics {
        /**
         * Functor that given an intersection finds next intersection in straight line through intersection.nextElement.
         */
        struct ContinueStraight {
            /**
             * Finds the closest intersection with intersection.nextElement
             * @param intersection
             * @return intersection unique pointer if intersection is found, else it returns nullptr
             */
            raytracer::geometry::Vector operator()(
                    const raytracer::geometry::PointOnFace &,
                    const raytracer::geometry::Vector &previousDirection,
                    const raytracer::geometry::Element &,
                    const raytracer::geometry::Element &,
                    const raytracer::physics::LaserRay &
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
                    const raytracer::geometry::MeshFunction &density,
                    const raytracer::geometry::MeshFunction &temperature,
                    const raytracer::geometry::MeshFunction &ionization,
                    const raytracer::physics::GradientCalculator &gradientCalculator,
                    const raytracer::physics::CollisionalFrequencyCalculator &collisionalFrequencyCalculator
            );

            /**
             * Tries to find another intersection based on Snells law, density (provides an index of rarefaction)
             * and gradient (provides edge direction). Could throw error no intersection found!
             * @param intersection
             * @param laserRay
             * @return found intersection or throw
             */
            raytracer::geometry::Vector operator()(
                    const raytracer::geometry::PointOnFace &pointOnFace,
                    const raytracer::geometry::Vector &previousDirection,
                    const raytracer::geometry::Element &previousElement,
                    const raytracer::geometry::Element &nextElement,
                    const raytracer::physics::LaserRay &laserRay
            );

        private:
            const raytracer::geometry::MeshFunction &density;
            const raytracer::geometry::MeshFunction &temperature;
            const raytracer::geometry::MeshFunction &ionization;
            const raytracer::physics::GradientCalculator &gradientCalculator;
            const raytracer::physics::CollisionalFrequencyCalculator &collisionalFrequencyCalculator;
        };
    }
}

#endif //RAYTRACER_REFRACTION_H
