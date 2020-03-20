#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Gradient.h"
#include "CollisionalFrequency.h"

namespace raytracer {
    /**
     * Functor that given an intersection finds next intersection in straight line through intersection.nextElement.
     */
    struct ContinueStraight {
        /**
         * Finds the closest intersection with intersection.nextElement
         * @param intersection
         * @return intersection unique pointer if intersection is found, else it returns nullptr
         */
        Vector operator()(
                const PointOnFace &,
                const Vector &previousDirection,
                const Element &,
                const Element &,
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
                const MeshFunction &density,
                const MeshFunction &temperature,
                const MeshFunction &ionization,
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
        Vector operator()(
                const PointOnFace &pointOnFace,
                const Vector &previousDirection,
                const Element &previousElement,
                const Element &nextElement,
                const LaserRay &laserRay
        );

    private:
        const MeshFunction &density;
        const MeshFunction &temperature;
        const MeshFunction &ionization;
        const GradientCalculator &gradientCalculator;
        const CollisionalFrequencyCalculator &collisionalFrequencyCalculator;
    };

}

#endif //RAYTRACER_REFRACTION_H
