#include "LaserPropagation.h"

namespace raytracer {
    namespace physics {
        StopAtCritical::StopAtCritical(const geometry::MeshFunction &density) :
                density(density) {}

        bool StopAtCritical::operator()(const geometry::Element &element, const LaserRay &laserRay) {
            auto currentDensity = density.getValue(element);
            auto criticalDensity = laserRay.getCriticalDensity();
            return currentDensity > criticalDensity.asDouble;
        }

        bool DontStop::operator()(const geometry::Element &, const LaserRay &) {
            return false;
        }

        SnellsLaw::SnellsLaw(
                const geometry::MeshFunction &density,
                const geometry::MeshFunction &temperature,
                const geometry::MeshFunction &ionization,
                const GradientCalculator &gradientCalculator,
                const CollisionalFrequencyCalculator &collisionalFrequencyCalculator
        ) :
                density(density),
                temperature(temperature),
                ionization(ionization),
                gradientCalculator(gradientCalculator),
                collisionalFrequencyCalculator(collisionalFrequencyCalculator) {}


        geometry::Vector SnellsLaw::operator()(
                const geometry::PointOnFace &pointOnFace,
                const geometry::Vector &previousDirection,
                const geometry::Element &previousElement,
                const geometry::Element &nextElement,
                const LaserRay &laserRay
        ) {
            const auto rho1 = Density{density.getValue(previousElement)};
            const auto rho2 = Density{density.getValue(nextElement)};

            const auto T1 = Temperature{temperature.getValue(previousElement)};
            const auto T2 = Temperature{temperature.getValue(nextElement)};

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
                return r * l + (r * c - std::sqrt(root)) * n;
            } else {
                return l + 2 * c * n;
            }
        }

        geometry::PointOnFace
        intersectStraight(
                const geometry::PointOnFace &pointOnFace,
                const geometry::Vector &direction,
                const geometry::Element &nextElement,
                const LaserRay &
        ) {
            auto newPointOnFace = geometry::findClosestIntersection(
                    {pointOnFace.point, direction},
                    nextElement.getFaces(),
                    pointOnFace.face
            );
            if (!newPointOnFace) throw std::logic_error("No intersection found, but it should definitely exist!");
            return *newPointOnFace;
        }
    }
}

