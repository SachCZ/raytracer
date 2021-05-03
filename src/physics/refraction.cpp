#include "laser.h"
#include "constants.h"
#include "propagation.h"
#include "refraction.h"
#include <memory>
#include <utility>
#include <utility.h>
#include <stdexcept>

namespace raytracer {
    SnellsLaw::SnellsLaw(
            const MeshFunc *refractIndex,
            Marker *reflectMarker,
            Vector *reflectDirection,
            const MeshFunc* density,
            const double* critDens
    ) :
            refractIndex(refractIndex),
            reflectMarker(reflectMarker),
            reflectDirection(reflectDirection),
            density(density),
            critDens(critDens)
            {}

    Vector SnellsLaw::operator()(
            const PointOnFace &pointOnFace,
            const Vector &previousDirection,
            const Element *previousElement,
            const Element *nextElement
    ) {
        if (!nextElement) return {0, 0};
        Vector gradient{};
        try {
            gradient = gradCalc(pointOnFace);
        } catch (const std::logic_error &error) {
            if (reflectDirection) {
                gradient = *reflectDirection;
            } else {
                gradient = previousDirection;
            }
        }
        if (gradient.getNorm() == 0) {
            if (reflectDirection) {
                gradient = *reflectDirection;
            } else {
                gradient = previousDirection;
            }
        }

        const double n1 = previousElement ? refractIndex->getValue(*previousElement) : 1.0;
        const double n2 = refractIndex->getValue(*nextElement);

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
        if (root < 0 || n2 <= 0 || (n2 > n1 && critDens && density->getValue(*nextElement) > *critDens)) {//Reflection
            if (
                    (reflectDirection && *reflectDirection * previousDirection < 0) ||
                    (!reflectDirection && gradient * previousDirection < 0)
                    ) {
                result = previousDirection;
            } else {
                if (reflectMarker) reflectMarker->mark(pointOnFace);
                result = l + 2 * c * n;
            }
        } else {
            result = r * l + (r * c - sqrt(root)) * n;
        }
        if (std::isnan(result.x) || std::isnan(result.y)) {
            throw std::logic_error("Snell's law generated nan direction, something is wrong!");
        }
        return result;
    }

    Density calcCritDens(const Length &wavelength) {
        auto m_e = constants::electron_mass;
        auto c = constants::speed_of_light;
        auto e = constants::electron_charge;

        auto constant = m_e * M_PI * std::pow(c, 2) / std::pow(e, 2);

        return {constant * std::pow(wavelength.asDouble, -2)};
    }

    void Marker::mark(const PointOnFace &pointOnFace) {
        marked.insert(pointOnFace.id);
    }

    void Marker::unmark(const PointOnFace &pointOnFace) {
        marked.erase(pointOnFace.id);
    }

    bool Marker::isMarked(const PointOnFace &pointOnFace) const {
        return marked.find(pointOnFace.id) != marked.end();
    }

    Vector
    ContinueStraight::operator()(const PointOnFace &, const Vector &previousDirection, const Element *,
                                 const Element *) {
        return previousDirection;
    }

    double calcRefractIndex(double density, const Length &wavelength, double collFreq) {
        auto permittivity = impl::calcPermittivity(density, wavelength, collFreq);
        if (permittivity.real() < 0) return 0;
        auto root = std::sqrt(permittivity);
        if (std::isnan(root.real())) {
            throw std::logic_error("Nan index of refraction!");
        }
        return root.real();
    }

    std::complex<double> impl::calcPermittivity(double density, const Length &wavelength, double collFreq) {
        auto nu_ei = collFreq;
        auto n_e = density;
        auto m_e = constants::electron_mass;
        auto e = constants::electron_charge;
        auto omega = 2 * M_PI * constants::speed_of_light / wavelength.asDouble;
        auto omega_p2 = 4 * M_PI * e * e * n_e / m_e;

        auto term = omega_p2 / (omega * omega + nu_ei * nu_ei);
        return {1 - term, nu_ei / omega * term};
    }

    double calcInvBremssCoeff(double density, const Length &wavelength, double collFreq) {
        auto eps = impl::calcPermittivity(density, wavelength, collFreq);
        return 4 * M_PI / wavelength.asDouble * std::sqrt(eps).imag();
    }
}