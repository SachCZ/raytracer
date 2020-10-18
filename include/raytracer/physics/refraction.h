#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include <geometry.h>
#include "gradient.h"
#include "collisional_frequency.h"


namespace raytracer {

    /**
     * Rule how a refractive index is calculated
     */
    class RefractiveIndex {
    public:
        /**
         * Override this
         * @param density
         * @param collisionFrequency
         * @param wavelength
         * @return
         */
        virtual double getRefractiveIndex(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        ) const = 0;
    };

    /**
     * Rule how a bremsstrahlung coeff is calculated
     */
    class BremsstrahlungCoeff {
    public:
        /**
         * Override this
         * @param density
         * @param collisionFrequency
         * @param wavelength
         * @return
         */
        virtual double getInverseBremsstrahlungCoeff(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        ) const = 0;
    };

    /**
     * Rule how critical density is calculated
     */
    class CriticalDensity {
    public:
        /**
         * Override this
         * @param wavelength
         * @return
         */
        virtual Density getCriticalDensity(const Length& wavelength) const = 0;
    };

    /**
     * The classic formula to calculate critical density
     */
    class ClassicCriticalDensity : public CriticalDensity {
    public:
        Density getCriticalDensity(const Length& wavelength) const override;
    };

    /**
     * ColdPlasma approximation provides both RefractiveIndex and BremsstrahlungCoeff
     */
    class ColdPlasma : public RefractiveIndex, public BremsstrahlungCoeff {
    public:
        /**
         * Calculate the index of refraction based on current density, collisional frequency.
         * @param density at which the refractive index is to be calculated
         * @param collisionFrequency current collisional frequency
         * @return refractive index
         */
        double getRefractiveIndex(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        ) const override;

        static std::complex<double> getPermittivity(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        );

        double getInverseBremsstrahlungCoeff(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        ) const override;
    };

    /**
     * Class used to mark elements that have some property. Actually it is just a set internally.
     */
    class Marker {
    public:
        /**
         * Mark an Element.
         * @param element
         */
        void mark(const Element &element, const PointOnFace &pointOnFace);

        /**
         * Unmark an Element
         * @param element
         */
        void unmark(const Element &element, const PointOnFace &pointOnFace);

        /**
         * Check whether an Element is marked by this marker.
         * @param element
         * @return
         */
        bool isMarked(const Element &element, const PointOnFace &pointOnFace) const;

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
                const Element &
        );
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
                const RefractiveIndex &refractiveIndexCalculator,
                const Length &wavelength,
                Marker *reflectedMarker = nullptr
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
                const Element &nextElement
        );

    private:
        const MeshFunction &density;
        const MeshFunction &temperature;
        const MeshFunction &ionization;
        const Gradient &gradientCalculator;
        const CollisionalFrequency &collisionalFrequencyCalculator;
        const RefractiveIndex &refractiveIndexCalculator;
        Length wavelength;
        Marker *reflectedMarker;
    };

    /**
     * @}
     */
}

#endif //RAYTRACER_REFRACTION_H
