#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include <geometry.h>
#include "gradient.h"
#include "collisional_frequency.h"
#include "constants.h"
#include <complex>
#include <utility>
#include <utility.h>
#include <set>


namespace raytracer {
    namespace impl {
        Vector calcRayReflect(const Vector &unitInterfaceNormal, const Vector &unitIncDir);

        Vector calcRayBend(const Vector &unitInterfaceNormal, const Vector &unitIncDir, double n1, double n2);

        bool shouldReflect(const Vector &unitInterfaceNormal, const Vector &unitIncDir, double n1, double n2);
    }

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

    template<typename MeshFunc>
    struct ReflectOnCritical {
        ReflectOnCritical(
                const Mesh *mesh,
                const MeshFunc &refractIndex,
                const MeshFunc &dens,
                double critDens,
                const Gradient *gradCalc,
                Marker *marker = nullptr,
                const Vector *fallbackGrad = nullptr
        ) : mesh(mesh), refractIndex(refractIndex), dens(dens),
            critDens(critDens), gradCalc(gradCalc), marker(marker), fallbackGrad(fallbackGrad) {}

        ReflectOnCritical() = default;

        tl::optional<Vector> operator()(
                const PointOnFace &pointOnFace,
                const Vector &direction
        ) {
            const auto nextElement = mesh->getFaceDirAdjElement(pointOnFace.face, direction);
            //const auto prevElement = mesh->getFaceDirAdjElement(pointOnFace.face, -1 * direction);
            if (!nextElement) return {};
            //const double n1 = prevElement ? refractIndex->getValue(*prevElement) : 1.0;
            //const double n2 = refractIndex->getValue(*nextElement);
            if (dens[nextElement->getId()] > critDens) {
                auto gradient = gradCalc->get(pointOnFace);
                if (!gradient || gradient.value().getNorm() == 0) {
                    if (fallbackGrad) {
                        gradient = *fallbackGrad;
                    } else {
                        return {};
                    }
                }
                auto unitGrad = 1 / gradient.value().getNorm() * gradient.value();
                auto unitDir = 1 / direction.getNorm() * direction;
                if (direction * gradient.value() < 0) {
                    return direction;
                } else {
                    if (marker) {
                        marker->mark(pointOnFace);
                    }
                    return impl::calcRayReflect(unitGrad, unitDir);
                }
            } else {
                return {};
            }
        }

    private:
        const Mesh *mesh{};
        const MeshFunc refractIndex{};
        const MeshFunc dens{};
        double critDens{};
        const Gradient *gradCalc{};
        Marker *marker{};
        const Vector *fallbackGrad{};
    };

    /**
     * Functor that finds new direction base on the Snells's law.
     */
    template<typename MeshFunc>
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
                const MeshFunc &refractIndex,
                const Gradient *gradCalc,
                Vector *fallbackGrad = nullptr
        ) :
                mesh(mesh),
                refractIndex(refractIndex),
                gradCalc(gradCalc),
                fallbackGrad(fallbackGrad) {}

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
                const Vector &direction
        ) {
            const auto previousElement = mesh->getFaceDirAdjElement(pointOnFace.face, -1 * direction);
            const auto nextElement = mesh->getFaceDirAdjElement(pointOnFace.face, direction);
            if (!nextElement) return {};
            auto gradient = gradCalc->get(pointOnFace);
            if (!gradient || gradient.value().getNorm() == 0) {
                if (fallbackGrad) {
                    gradient = *fallbackGrad;
                } else {
                    return {};
                }
            }

            const double n2 = refractIndex[nextElement->getId()];
            const double n1 = previousElement ? refractIndex[previousElement->getId()] : std::min(n2, 1.0);

            auto unitGrad = 1 / gradient.value().getNorm() * gradient.value();
            auto unitDir = 1 / direction.getNorm() * direction;
            return impl::calcRayBend(unitGrad, unitDir, n1, n2);
        }

    private:
        const Mesh *mesh{};
        const MeshFunc refractIndex{};
        const Gradient *gradCalc{};
        Vector *fallbackGrad{};
    };

    template<typename MeshFunc>
    struct TotalReflect {
        explicit TotalReflect(
                const Mesh *mesh,
                const MeshFunc &refractIndex,
                const Gradient *gradCalc,
                Marker *reflectMarker = nullptr,
                Vector *fallbackGrad = nullptr
        ) :
                mesh(mesh),
                refractIndex(refractIndex),
                gradCalc(gradCalc),
                reflectMarker(reflectMarker),
                fallbackGrad(fallbackGrad) {}

        TotalReflect() = default;

        tl::optional<Vector> operator()(
                const PointOnFace &pointOnFace,
                const Vector &direction
        ) {
            const auto previousElement = mesh->getFaceDirAdjElement(pointOnFace.face, -1 * direction);
            const auto nextElement = mesh->getFaceDirAdjElement(pointOnFace.face, direction);
            if (!nextElement) return {};
            auto gradient = gradCalc->get(pointOnFace);
            if (!gradient || gradient.value().getNorm() == 0) {
                if (fallbackGrad) {
                    gradient = *fallbackGrad;
                } else {
                    return {};
                }
            }

            const double n2 = refractIndex[nextElement->getId()];
            const double n1 = previousElement ? refractIndex[previousElement->getId()] : std::min(n2, 1.0);

            auto unitGrad = 1 / gradient.value().getNorm() * gradient.value();
            auto unitDir = 1 / direction.getNorm() * direction;
            if (impl::shouldReflect(unitGrad, unitDir, n1, n2)) {
                if (gradient.value() * direction < 0) {
                    return direction;
                } else {
                    if (reflectMarker) reflectMarker->mark(pointOnFace);
                    return impl::calcRayReflect(unitGrad, unitDir);
                }
            } else {
                return {};
            }
        }

    private:
        const Mesh *mesh{};
        const MeshFunc refractIndex{};
        const Gradient *gradCalc{};
        Marker *reflectMarker{};
        Vector *fallbackGrad{};
    };
}

#endif //RAYTRACER_REFRACTION_H
