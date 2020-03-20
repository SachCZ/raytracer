#include "Propagation.h"
#include "Refraction.h"

namespace raytracer {
    namespace physics {
        SnellsLaw::SnellsLaw(
                const raytracer::geometry::MeshFunction &density,
                const raytracer::geometry::MeshFunction &temperature,
                const raytracer::geometry::MeshFunction &ionization,
                const raytracer::physics::GradientCalculator &gradientCalculator,
                const raytracer::physics::CollisionalFrequencyCalculator &collisionalFrequencyCalculator
        ) :
                density(density),
                temperature(temperature),
                ionization(ionization),
                gradientCalculator(gradientCalculator),
                collisionalFrequencyCalculator(collisionalFrequencyCalculator) {}

        raytracer::geometry::Vector SnellsLaw::operator()(
                const raytracer::geometry::PointOnFace &pointOnFace,
                const raytracer::geometry::Vector &previousDirection,
                const raytracer::geometry::Element &previousElement,
                const raytracer::geometry::Element &nextElement,
                const raytracer::physics::LaserRay &laserRay
        ) {
            const auto rho1 = raytracer::physics::Density{density.getValue(previousElement)};
            const auto rho2 = raytracer::physics::Density{density.getValue(nextElement)};

            const auto T1 = raytracer::physics::Temperature{temperature.getValue(previousElement)};
            const auto T2 = raytracer::physics::Temperature{temperature.getValue(nextElement)};

            const auto Z1 = ionization.getValue(previousElement);
            const auto Z2 = ionization.getValue(nextElement);

            const auto nu_ei_1 = collisionalFrequencyCalculator.getCollisionalFrequency(rho1, T1, laserRay.wavelength,
                                                                                        Z1);
            const auto nu_ei_2 = collisionalFrequencyCalculator.getCollisionalFrequency(rho2, T2, laserRay.wavelength,
                                                                                        Z2);

            const double n1 = laserRay.getRefractiveIndex(rho1, nu_ei_1);
            const double n2 = laserRay.getRefractiveIndex(rho2, nu_ei_2);

            const auto gradient = gradientCalculator.getGradient(pointOnFace, previousElement, nextElement);

            const auto l = 1 / previousDirection.getNorm() * previousDirection;
            auto n = 1 / gradient.getNorm() * gradient;
            auto c = (-1) * n * l;
            if (c < 0) {
                c = -c;
                n = (-1) * n;
            }
            const double r = n1 / n2;

            auto root = 1 - r * r * (1 - c * c);
            if (root > 0) {
                return r * l + (r * c - sqrt(root)) * n;
            } else {
                return l + 2 * c * n;
            }
        }
    }
}