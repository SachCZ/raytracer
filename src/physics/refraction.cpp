#include "laser.h"
#include "constants.h"
#include "propagation.h"
#include "refraction.h"

namespace raytracer {
    SnellsLaw::SnellsLaw(
            const MeshFunction &density,
            const MeshFunction &temperature,
            const MeshFunction &ionization,
            const Gradient &gradientCalculator,
            const CollisionalFrequency &collisionalFrequencyCalculator,
            const RefractiveIndex& refractiveIndexCalculator,
            const Length& wavelength,
            Marker* reflectedMarker
    ) :
            density(density),
            temperature(temperature),
            ionization(ionization),
            gradientCalculator(gradientCalculator),
            collisionalFrequencyCalculator(collisionalFrequencyCalculator),
            refractiveIndexCalculator(refractiveIndexCalculator),
            wavelength(wavelength),
            reflectedMarker(reflectedMarker)
    {}

    Vector SnellsLaw::operator()(
            const PointOnFace &pointOnFace,
            const Vector &previousDirection,
            const Element &previousElement,
            const Element &nextElement
    ) {
        auto gradient = gradientCalculator.get(pointOnFace, previousElement, nextElement);
        const auto rho1 = Density{density.getValue(previousElement)};
        const auto rho2 = Density{density.getValue(nextElement)};

        const auto T1 = Temperature{temperature.getValue(previousElement)};
        const auto T2 = Temperature{temperature.getValue(nextElement)};

        const auto Z1 = ionization.getValue(previousElement);
        const auto Z2 = ionization.getValue(nextElement);

        const auto nu_ei_1 = collisionalFrequencyCalculator.get(rho1, T1, wavelength, Z1);
        const auto nu_ei_2 = collisionalFrequencyCalculator.get(rho2, T2, wavelength, Z2);

        const double n1 = refractiveIndexCalculator.getRefractiveIndex(rho1, nu_ei_1, wavelength);
        const double n2 = refractiveIndexCalculator.getRefractiveIndex(rho2, nu_ei_2, wavelength);

        const auto l = 1 / previousDirection.getNorm() * previousDirection;

        auto n = 1 / gradient.getNorm() * gradient;
        auto c = (-1) * n * l;
        if (c < 0) {
            c = -c;
            n = (-1) * n;
        }
        const double r = n1 / n2;

        auto root = 1 - r * r * (1 - c * c);
        Vector result{};
        if (root > 0) {
            result = r * l + (r * c - sqrt(root)) * n;
        } else {//Reflection
            if (gradient*previousDirection < 0){//Against gradient
                result =  previousDirection;
            }
            else {
                if(reflectedMarker) reflectedMarker->mark(previousElement, pointOnFace);
                result = l + 2 * c * n;
            }
        }
        if (std::isnan(result.x) || std::isnan(result.y)){
            throw std::logic_error("Snell's law generated nan direction, something is wrong!");
        }
        return result;
    }

    Density ClassicCriticalDensity::getCriticalDensity(const Length &wavelength) const {
        auto m_e = constants::electron_mass;
        auto c = constants::speed_of_light;
        auto e = constants::electron_charge;

        auto constant = m_e * M_PI * std::pow(c, 2) / std::pow(e, 2);

        return {constant * std::pow(wavelength.asDouble, -2)};
    }

    double ColdPlasma::getRefractiveIndex(const Density &density, const Frequency &collisionFrequency,
                                          const Length &wavelength) const {
        auto permittivity = getPermittivity(density, collisionFrequency, wavelength);
        if (permittivity.real() < 0) return 0;
        auto root = std::sqrt(permittivity);
        if (std::isnan(root.real())) {
            throw std::logic_error("Nan index of refraction!");
        }
        return root.real();
    }

    std::complex<double>
    ColdPlasma::getPermittivity(const Density &density, const Frequency &collisionFrequency, const Length &wavelength) {
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

    double ColdPlasma::getInverseBremsstrahlungCoeff(const Density &density, const Frequency &collisionFrequency,
                                                     const Length &wavelength) const {
        auto eps = getPermittivity(density, collisionFrequency, wavelength);
        return 4 * M_PI / wavelength.asDouble * std::sqrt(eps).imag();
    }

    void Marker::mark(const Element &element, const PointOnFace &pointOnFace) {
        marked.insert(std::make_pair(element.getId(), pointOnFace.id));
    }

    void Marker::unmark(const Element &element, const PointOnFace &pointOnFace) {
        marked.erase(std::make_pair(element.getId(), pointOnFace.id));
    }

    bool Marker::isMarked(const Element &element, const PointOnFace &pointOnFace) const {
        return marked.find(std::make_pair(element.getId(), pointOnFace.id)) != marked.end();
    }

    Vector
    ContinueStraight::operator()(const PointOnFace &, const Vector &previousDirection, const Element &, const Element &) {
        return previousDirection;
    }
}