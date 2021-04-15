#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include <geometry.h>
#include "gradient.h"
#include "collisional_frequency.h"
#include "constants.h"
#include <complex>


namespace raytracer {

    /**
     * The classic formula to calculate critical density
     * @param wavelength
     */
    Density calcCritDens(const Length &wavelength);

    double calcRefractIndex(double density, const Length &wavelength, double collFreq);

    double calcInvBremssCoeff(double density, const Length &wavelength, double collFreq);

    namespace impl {
        std::complex<double> calcPermittivity(double density, const Length &wavelength, double collFreq);
    }

    /**
     * Class used to mark elements that have some property. Actually it is just a set internally.
     */
    class Marker {
    public:
        /**
         * Mark a PointOnFace
         * @param pointOnFace
         */
        void mark(const PointOnFace &pointOnFace);

        /**
         * Remove the mark
         * @param pointOnFace
         */
        void unmark(const PointOnFace &pointOnFace);

        /**
         * Check whether an Element is marked by this marker.
         * @param pointOnFace
         * @return
         */
        bool isMarked(const PointOnFace &pointOnFace) const;

    private:
        std::set<int> marked;
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
        explicit SnellsLaw(
                const MeshFunc *refractIndex,
                Marker *reflectMarker = nullptr,
                Vector *reflectDirection = nullptr,
                const MeshFunc *density = nullptr,
                const double * critDens = nullptr
        );

        SnellsLaw() = default;

        void setGradCalc(const Gradient& gradient){
            gradCalc = gradient;
        }

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
        Gradient gradCalc;
        const MeshFunc *refractIndex{};
        Marker *reflectMarker{};
        Vector *reflectDirection{};
        const MeshFunc *density{};
        const double * critDens{};
    };
}

#endif //RAYTRACER_REFRACTION_H
