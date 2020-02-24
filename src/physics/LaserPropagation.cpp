#include "LaserPropagation.h"

namespace raytracer {
    namespace physics {
        StopAtCritical::StopAtCritical(const geometry::MeshFunction &density) :
                density(density) {}

        bool StopAtCritical::operator()(const geometry::Intersection &intersection, const LaserRay &laserRay) {
            const auto element = *intersection.nextElement;

            auto currentDensity = density.getValue(element);
            auto criticalDensity = laserRay.getCriticalDensity();
            return currentDensity > criticalDensity.asDouble;
        }

        std::unique_ptr<geometry::Intersection>
        ContinueStraight::operator()(const geometry::Intersection &intersection, const LaserRay &) {
            return findClosestIntersection(
                    intersection.orientation,
                    intersection.nextElement->getFaces(),
                    intersection.face);
        }

        bool DontStop::operator()(const geometry::Intersection &, const LaserRay &) {
            return false;
        }

        SnellsLaw::SnellsLaw(
                const geometry::MeshFunction &density,
                const geometry::MeshFunction& temperature,
                const geometry::MeshFunction& ionization,
                const GradientCalculator &gradientCalculator,
                const CollisionalFrequencyCalculator& collisionalFrequencyCalculator
        ) :
        density(density),
        temperature(temperature),
        ionization(ionization),
        gradientCalculator(gradientCalculator),
        collisionalFrequencyCalculator(collisionalFrequencyCalculator) {}

        std::unique_ptr<geometry::Intersection>
        SnellsLaw::operator()(const geometry::Intersection &intersection, const LaserRay &laserRay) {
            const auto previousElement = intersection.previousElement;
            const auto nextElement = intersection.nextElement;

            if (!previousElement) {
                return findClosestIntersection(intersection.orientation, nextElement->getFaces(),
                                               intersection.face);
            }

            auto orientation = geometry::HalfLine{
                    intersection.orientation.point,
                    getDirection(intersection, laserRay)
            };
            auto newIntersection = findClosestIntersection(
                    orientation,
                    nextElement->getFaces(),
                    intersection.face);
            if (!newIntersection) {
                //The ray must have reflected unexpectedly
                newIntersection = findClosestIntersection(
                        orientation,
                        previousElement->getFaces(),
                        intersection.face);
                if (newIntersection){
                    newIntersection->nextElement = previousElement;
                }
                else {
                    throw std::logic_error("No intersection found");
                }
            }
            return newIntersection;
        }

        geometry::Vector SnellsLaw::getDirection(const geometry::Intersection &intersection, const LaserRay &laserRay) {
            const auto rho1 = Density{density.getValue(*intersection.previousElement)};
            const auto rho2 = Density{density.getValue(*intersection.nextElement)};

            const auto T1 = Temperature{temperature.getValue(*intersection.previousElement)};
            const auto T2 = Temperature{temperature.getValue(*intersection.nextElement)};

            const auto Z1 = ionization.getValue(*intersection.previousElement);
            const auto Z2 = ionization.getValue(*intersection.nextElement);

            const auto nu_ei_1 = collisionalFrequencyCalculator.getCollisionalFrequency(rho1, T1, laserRay.wavelength, Z1);
            const auto nu_ei_2 = collisionalFrequencyCalculator.getCollisionalFrequency(rho2, T2, laserRay.wavelength, Z2);

            const double n1 = laserRay.getRefractiveIndex(rho1, nu_ei_1);
            const double n2 = laserRay.getRefractiveIndex(rho2, nu_ei_2);

            const auto gradient = gradientCalculator.getGradient(intersection);
            const auto &direction = intersection.orientation.direction;

            const auto l = 1 / direction.getNorm() * direction;
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
    }
}

