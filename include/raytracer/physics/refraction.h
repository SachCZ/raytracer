#ifndef RAYTRACER_REFRACTION_H
#define RAYTRACER_REFRACTION_H

#include "mesh_function.h"
#include "gradient.h"
#include "collisional_frequency.h"

namespace raytracer {

    class RefractiveIndex {
    public:
        virtual double getRefractiveIndex(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        ) const = 0;
    };

    class BremsstrahlungCoeff {
    public:
        virtual double getInverseBremsstrahlungCoeff(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        ) const = 0;
    };

    class CriticalDensity {
    public:
        virtual Density getCriticalDensity(const Length& wavelength) const = 0;
    };

    class ClassicCriticalDensity : public CriticalDensity {
    public:
        Density getCriticalDensity(const Length& wavelength) const override {
            auto m_e = constants::electron_mass;
            auto c = constants::speed_of_light;
            auto e = constants::electron_charge;

            auto constant = m_e * M_PI * std::pow(c, 2) / std::pow(e, 2);

            return {constant * std::pow(wavelength.asDouble, -2)};
        }
    };

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
        ) const override {
            auto permittivity = getPermittivity(density, collisionFrequency, wavelength);
            if (permittivity.real() < 0) return 0;
            auto root = std::sqrt(permittivity);
            if (std::isnan(root.real())) {
                throw std::logic_error("Nan index of refraction!");
            }
            return root.real();
        }

        static std::complex<double> getPermittivity(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length wavelength
        ) {
            using namespace std::complex_literals;

            auto nu_ei = collisionFrequency.asDouble;
            auto n_e = density.asDouble;
            auto m_e = constants::electron_mass;
            auto e = constants::electron_charge;
            auto omega = 2 * M_PI * constants::speed_of_light / wavelength.asDouble;
            auto omega_p2 = 4 * M_PI * e * e * n_e / m_e;

            auto term = omega_p2 / (omega * omega + nu_ei * nu_ei);
            return 1 - term + 1i * nu_ei / omega * term;
        }

        double getInverseBremsstrahlungCoeff(
                const Density &density,
                const Frequency &collisionFrequency,
                const Length &wavelength
        ) const override {
            auto eps = getPermittivity(density, collisionFrequency, wavelength);
            return 4 * M_PI / wavelength.asDouble * std::sqrt(eps).imag();
        }
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
        void mark(const Element &element, const PointOnFace &pointOnFace) {
            marked.insert(std::make_pair(element.getId(), pointOnFace.id));
        }

        /**
         * Unmark an Element
         * @param element
         */
        void unmark(const Element &element, const PointOnFace &pointOnFace) {
            marked.erase(std::make_pair(element.getId(), pointOnFace.id));
        }

        /**
         * Check whether an Element is marked by this marker.
         * @param element
         * @return
         */
        bool isMarked(const Element &element, const PointOnFace &pointOnFace) const {
            return marked.find(std::make_pair(element.getId(), pointOnFace.id)) != marked.end();
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
                const Element &
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
