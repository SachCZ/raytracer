#ifndef RAYTRACER_LASER_RAY_H
#define RAYTRACER_LASER_RAY_H

#include <vector>

#include "Ray.h"
#include "Magnitudes.h"
#include "Constants.h"

namespace raytracer {

    /**
     * Class representing a single LaserRay. It has associated energy but no width for now.
     */
    class LaserRay {
    public:
        /** The point from which the LaserRay originates. */
        Point startPoint{};
        /** The initial direction of the LaserRay. */
        Vector direction{};
        /** Absolute energy carried by the LaserRay. */
        Energy energy{};
        /** Wavelength of the LaserRay in cm */
        Length wavelength{};
        /** Unique identifier of the LaserRay */
        int id{};

        /** The electron critical density of the LaserRay */
        Density getCriticalDensity() const;

        /**
         * Calculate the index of refraction based on current density, collisional frequency.
         * @param density at which the refractive index is to be calculated
         * @param collisionFrequency current collisional frequency
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
                const Frequency &collisionFrequency) const;

        /**
         * Wrapper around Ray::findIntersections().
         * It generates a ray based on the LaserRay properties and saves the result to LaserRay::intersections.
         *
         * @tparam IntersectionFunction
         * @tparam StopCondition
         * @param mesh to be intersected
         * @param findDirection will be propagated to Ray::findIntersections() as is.
         * @param findIntersection will be propagated to Ray::findIntersections() as is.
         * @param stopCondition will be propagated to Ray::findIntersections() as is.
         */
        template<typename DirectionFunction, typename IntersectionFunction, typename StopCondition>
        void generateIntersections(
                const Mesh &mesh,
                DirectionFunction findDirection,
                IntersectionFunction findIntersection,
                StopCondition stopCondition) {
            Ray ray(HalfLine{this->startPoint, this->direction});

            this->intersections = ray.findIntersections(mesh, findDirection, findIntersection, stopCondition);
        }

        /** Sequence of all intersections with given Mesh.
         * This is empty if no generateIntersections was called.*/
        std::vector<Intersection> intersections;
    };

}

#endif //RAYTRACER_LASER_RAY_H
