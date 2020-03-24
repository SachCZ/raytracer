#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Gradient.h"
#include "CollisionalFrequency.h"

namespace raytracer {
    /**
     * \addtogroup directionFinders
     * @{
     */

    /**
     * Functor that returns always the previousDirection.
     */
    struct ContinueStraight {
        /**
         * Function to be used in Laser::generateIntersections() as findDirection. It returns the previousDirection
         * given, effectively continuing in a straight line.
         * @param previousDirection
         * @return previousDirection
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
 * Functor that finds new direction base on the Snells's law.
 */
    struct SnellsLaw {
        /**
         * Construct the SnellsLaw using the density, temperature and ionization function references.
         * Provide gradientCalculator calculate the plane of rarefaction. Provide collisionalFrequencyCalculator
         * to evaluate the index of rarefaction.
         * Density, temperature and ionization values are expected to change.
         * @param density
         * @param temperature
         * @param ionization
         * @param gradientCalculator
         * @param collisionalFrequencyCalculator
         */
        explicit SnellsLaw(
                const MeshFunction &density,
                const MeshFunction &temperature,
                const MeshFunction &ionization,
                const Gradient &gradientCalculator,
                const CollisionalFrequency &collisionalFrequencyCalculator,
                Marker* reflected = nullptr
        );


        /**
         * Function to be used in Laser::generateIntersections() as findDirection.
         * Finds new direction based on Snells law between the two Element, density (provides an index of rarefaction)
         * and gradient (provides edge direction). Could throw error no intersection found!
         *
         * @param pointOnFace
         * @param previousDirection
         * @param previousElement
         * @param nextElement
         * @param laserRay
         * @return new direction based on Snells law.
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
        const Gradient &gradientCalculator;
        const CollisionalFrequency &collisionalFrequencyCalculator;
        Marker* reflected;
    };

    /**
     * @}
     */
}

#endif //RAYTRACER_REFRACTION_H
