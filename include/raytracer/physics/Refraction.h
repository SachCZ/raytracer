#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Gradient.h"
#include "CollisionalFrequency.h"

namespace raytracer {

    /**
     * Class used to mark elements that have some property. Actually it is just a set internally.
     */
    class Marker {
    public:
        /**
         * Mark an Element.
         * @param element
         */
        void mark(const Element& element, const LaserRay& laserRay){
            marked.insert(std::make_pair(element.getId(), laserRay.id));
        }
        /**
         * Unmark an Element
         * @param element
         */
        void unmark(const Element& element, const LaserRay& laserRay) {
            marked.erase(std::make_pair(element.getId(), laserRay.id));
        }

        /**
         * Check whether an Element is marked by this marker.
         * @param element
         * @return
         */
        bool isMarked(const Element& element, const LaserRay& laserRay) const {
            return marked.find(std::make_pair(element.getId(), laserRay.id)) != marked.end();
        }
    private:
        std::set<std::pair<int, int>> marked;
    };

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
         * @param reflectedMarker Marker where after using SnellsLaw all Element where the ray was reflected will
         * will be marked - optional parameter
         */
        explicit SnellsLaw(
                const MeshFunction &density,
                const MeshFunction &temperature,
                const MeshFunction &ionization,
                const Gradient &gradientCalculator,
                const CollisionalFrequency &collisionalFrequencyCalculator,
                Marker* reflectedMarker = nullptr
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
        Marker* reflectedMarker;
    };

    /**
     * @}
     */
}

#endif //RAYTRACER_REFRACTION_H
