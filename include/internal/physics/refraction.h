#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include <geometry.h>
#include "gradient.h"
#include "collisional_frequency.h"
#include "constants.h"
#include <complex>
#include <utility>
#include <utility.h>
#include <mesh_function.h>


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
        tl::optional<Vector> operator()(
                const PointOnFace &,
                const Vector &previousDirection
        );
    };

    struct ReflectOnCritical {
        ReflectOnCritical(
                const Mesh *mesh,
                const MeshFunc *refractIndex,
                const MeshFunc *dens,
                double critDens,
                const Gradient* gradCalc,
                Marker *marker = nullptr,
                const Vector *fallbackGrad = nullptr
        )
                : mesh(mesh), refractIndex(refractIndex), dens(dens), critDens(critDens), gradCalc(gradCalc),
                marker(marker), fallbackGrad(fallbackGrad) {}

        ReflectOnCritical() = default;

        tl::optional<Vector> operator()(
                const PointOnFace &pointOnFace,
                const Vector &direction
        );

    private:
        const Mesh *mesh{};
        const MeshFunc* refractIndex{};
        const MeshFunc *dens{};
        double critDens{};
        const Gradient* gradCalc{};
        Marker *marker{};
        const Vector *fallbackGrad{};
    };

    /**
     * Functor that finds new direction base on the Snells's law.
     */
    struct SnellsLawBend {
        /**
         * Snell's law needs gradient and refractive index to calculate refraction. It can optionally mark
         * a PointOnFace at which a reflection occured
         * @param gradCalc
         * @param refractIndex - model
         * @param reflectMarker
         */
        explicit SnellsLawBend(
                const Mesh *mesh,
                const MeshFunc *refractIndex,
                const Gradient* gradient,
                Vector *fallbackGrad = nullptr
        );

        SnellsLawBend() = default;

        /**
         * Apply Snells law using the values calculated at previous and next elements
         *
         * @param pointOnFace
         * @param previousDirection
         * @param previousElement
         * @param nextElement
         * @return new direction based on Snells law.
         */
        tl::optional<Vector> operator()(
                const PointOnFace &pointOnFace,
                const Vector &previousDirection
        );

    private:
        const Mesh *mesh{};
        const MeshFunc *refractIndex{};
        const Gradient* gradCalc{};
        Vector *fallbackGrad{};
    };

    struct TotalReflect {
        explicit TotalReflect(
                const Mesh *mesh,
                const MeshFunc *refractIndex,
                const Gradient* gradCalc,
                Marker *reflectMarker = nullptr,
                Vector *fallbackGrad = nullptr
        );

        TotalReflect() = default;

        tl::optional<Vector> operator()(
                const PointOnFace &pointOnFace,
                const Vector &previousDirection
        );

    private:
        const Mesh *mesh{};
        const MeshFunc *refractIndex{};
        const Gradient* gradCalc{};
        Marker *reflectMarker{};
        Vector *fallbackGrad{};
    };
}

#endif //RAYTRACER_REFRACTION_H
