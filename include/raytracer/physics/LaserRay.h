#ifndef RAYTRACER_LASER_RAY_H
#define RAYTRACER_LASER_RAY_H

#include <vector>

#include "Ray.h"
#include "Magnitudes.h"
#include "Constants.h"

namespace raytracer {

    /**
     * Structure representing a single LaserRay. It has associated energy but no width for now.
     */
    struct LaserRay {
        /** The point from which the LaserRay originates. */
        Point startPoint{};
        /** The initial direction of the LaserRay. */
        Vector direction{};
        /** Absolute energy carried by the LaserRay. */
        Energy energy{};
        /** Wavelength of the LaserRay in cm */
        Length wavelength{};

        /** The electron critical density of the LaserRay */
        Density getCriticalDensity() const;

        /**
         * Calculate the index of refraction based on current density, collisional frequency.
         * @param density at which the refractive index is to be calculated
         * @return refractive index
         */
        double getRefractiveIndex(const Density &density, const Frequency &collisionFrequency) const;

        /**
         * Calculate the inverse bremsstrahlung coefficient base on current density and collisional frequency.
         * @param density
         * @param collisionFrequency
         * @return
         */
        double getInverseBremsstrahlungCoeff(const Density &density, const Frequency &collisionFrequency) const {
            auto eps = this->getPermittivity(density, collisionFrequency);
            return 4 * M_PI / this->wavelength.asDouble * std::sqrt(eps).imag();
        }

        /**
         * Calculate the permittivity based on current density and collisionFrequency
         * @param density
         * @param collisionFrequency
         * @return permittivity
         */
        std::complex<double> getPermittivity(
                const Density &density,
                const Frequency &collisionFrequency) const {
            using namespace std::complex_literals;

            auto nu_ei = collisionFrequency.asDouble;
            auto n_e = density.asDouble;
            auto m_e = constants::electron_mass;
            auto e = constants::electron_charge;
            auto omega = 2 * M_PI * constants::speed_of_light / this->wavelength.asDouble;
            auto omega_p2 = 4 * M_PI * e * e * n_e / m_e;

            auto term = omega_p2 / (omega * omega + nu_ei * nu_ei);
            return 1 - term + 1i * nu_ei / omega * term;
        }

        /** Wrapper around Ray::findIntersections.
         * It generates a ray based on the LaserRay properties and saves the result to LaserRay::intersections.
         *
         * @tparam IntersFunc function with signature (Intersection) -> std::unique_ptr<Intersection>
         * @tparam StopCondition function with signature (Intersection) -> bool
         * @param mesh to be intersected
         * @param findInters will be propagated to Ray::findIntersections as is.
         * @param stopCondition will be propagated to ray::findIntersections as is.
         */
        template<typename DirectionFunction, typename IntersFunc, typename StopCondition>
        void generateIntersections(
                const Mesh &mesh,
                DirectionFunction findDirection,
                IntersFunc findInters,
                StopCondition stopCondition) {
            Ray ray(HalfLine{this->startPoint, this->direction});

            this->intersections = ray.findIntersections(mesh, findDirection, findInters, stopCondition);
        }

        /** Sequence of all intersections with given Mesh.
         * This is empty if no generateIntersections was called.*/
        std::vector<Intersection> intersections;
    };

}

#endif //RAYTRACER_LASER_RAY_H
