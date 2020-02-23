#ifndef RAYTRACER_LASER_RAY_H
#define RAYTRACER_LASER_RAY_H

#include <vector>

#include "Ray.h"
#include "Magnitudes.h"
#include "Constants.h"

namespace raytracer {
    namespace physics {

        /**
         * Structure representing a single LaserRay. It has associated energy but no width for now.
         */
        struct LaserRay {
            /** The point from which the LaserRay originates. */
            geometry::Point startPoint{};
            /** The initial direction of the LaserRay. */
            geometry::Vector direction{};
            /** Absolute energy carried by the LaserRay. */
            Energy energy{};
            /** Wavelength of the LaserRay in cm */
            Length wavelength{};

            /** The electron critical density of the LaserRay */
            Density getCriticalDensity() const;

            /**
             * Calculate the index of refraction based on current density, collisionalFrequency
             * and critical density of this LaserRay.
             * @param density at which the refractive index is to be calculated
             * @return refractive index
             */
            double getRefractiveIndex(const Density &density, const Frequency &collisionFrequency) const;

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
                auto ne = density.asDouble;
                auto ne_crit = this->getCriticalDensity().asDouble;
                auto m_e = constants::electron_mass;
                auto e = constants::electron_charge;

                auto k = m_e / (4 * M_PI * e * e) * nu_ei * nu_ei;
                auto term = ne / (ne_crit + k);
                return 1 - term + 1i * std::sqrt(k / ne_crit) * term;
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
            template<typename IntersFunc, typename StopCondition>
            void generateIntersections(
                    const geometry::Mesh &mesh,
                    IntersFunc findInters,
                    StopCondition stopCondition) {
                geometry::Ray ray(geometry::HalfLine{this->startPoint, this->direction});

                this->intersections = ray.findIntersections(mesh, findInters, stopCondition);
            }

            /** Sequence of all intersections with given Mesh.
             * This is empty if no generateIntersections was called.*/
            std::vector<geometry::Intersection> intersections;
        };
    }
}

#endif //RAYTRACER_LASER_RAY_H
