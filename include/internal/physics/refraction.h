#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include <geometry.h>
#include "gradient.h"
#include "collisional_frequency.h"


namespace raytracer {

    /**
     * Rule how a refractive index is calculated
     */
    class RefractIndex {
    public:
        /**
         * Override this
         * @param density
         * @param collisionFrequency
         * @param wavelength
         * @return
         */
        virtual double getRefractiveIndex(const Element &element) const = 0;
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
        virtual double getInverseBremsstrahlungCoeff(const Element &element) const = 0;
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
    class ColdPlasma : public RefractIndex, public BremsstrahlungCoeff {
    public:
        /**
         * To model cold plasma and its properties one needs electron density, collisional frequency and
         * wavelength of the passing laser
         * @param density
         * @param collFreq - model
         * @param wavelength
         */
        ColdPlasma(const MeshFunc &density, const Length &wavelength, const MeshFunc *collFreq = nullptr);

        /**
         * Calculate the index of refraction based on current density, collisional frequency and wavelength.
         * @param density
         * @param collisionFrequency
         * @param wavelength
         * @return
         */
        double getRefractiveIndex(const Element &element) const override;

        /**
         * Calculate permittivity of the plasma
         * @param density
         * @param collisionFrequency
         * @param wavelength
         * @return permittivity
         */
        std::complex<double> getPermittivity(const Element &element) const;
        /**
         * Calculate bremsstrahlung coeff of the plasma
         * @param density
         * @param collisionFrequency
         * @param wavelength
         * @return bremsstrahlung coeff
         */
        double getInverseBremsstrahlungCoeff(const Element &element) const override;

    private:
        const MeshFunc& density;
        const MeshFunc* collFreq;
        const Length wavelength;
    };

    /**
     * Class used to mark elements that have some property. Actually it is just a set internally.
     */
    class Marker {
    public:
        /**
         * Mark a PointOnFace
         * @param element
         * @param pointOnFace
         */
        void mark(const Element &element, const PointOnFace &pointOnFace);

        /**
         * Remove the mark
         * @param element
         * @param pointOnFace
         */
        void unmark(const Element &element, const PointOnFace &pointOnFace);

        /**
         * Check whether an Element is marked by this marker.
         * @param element
         * @param pointOnFace
         * @return
         */
        bool isMarked(const Element &element, const PointOnFace &pointOnFace) const;

    private:
        std::set<std::pair<int, int>> marked;
    };

    /**
     * Functor that returns always the previousDirection.
     */
    struct ContinueStraight {
        /**
         * Return previousDirection
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
         * Snell's law needs gradient and refractive index to calculate refraction. It can optionally mark
         * a PointOnFace at which a reflection occured
         * @param gradCalc
         * @param refractIndex - model
         * @param reflectMarker
         */
        explicit SnellsLaw(const Gradient &gradCalc, const RefractIndex &refractIndex, Marker *reflectMarker = nullptr);


        /**
         * Apply Snells law using the values calculated at previous and next elements
         *
         * @param pointOnFace
         * @param previousDirection
         * @param previousElement
         * @param nextElement
         * @return new direction based on Snells law.
         */
        Vector operator()(
                const PointOnFace &pointOnFace,
                const Vector &previousDirection,
                const Element &previousElement,
                const Element &nextElement
        );

    private:
        const Gradient &gradCalc;
        const RefractIndex &refractIndex;
        Marker *reflectMarker;
    };
}

#endif //RAYTRACER_REFRACTION_H
