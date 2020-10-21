#include "laser.h"
#include "constants.h"
#include "propagation.h"
#include "refraction.h"

namespace raytracer {
    SnellsLaw::SnellsLaw(const Gradient &gradCalc, const RefractIndex &refractIndex,
                         Marker *reflectMarker) :
            gradCalc(gradCalc),
            refractIndex(refractIndex),
            reflectMarker(reflectMarker) {}

    Vector SnellsLaw::operator()(
            const PointOnFace &pointOnFace,
            const Vector &previousDirection,
            const Element &previousElement,
            const Element &nextElement
    ) {
        auto gradient = gradCalc.get(pointOnFace, previousElement, nextElement);

        const double n1 = refractIndex.getRefractiveIndex(previousElement);
        const double n2 = refractIndex.getRefractiveIndex(nextElement);

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
            if (gradient * previousDirection < 0) {//Against gradient
                result = previousDirection;
            } else {
                if (reflectMarker) reflectMarker->mark(previousElement, pointOnFace);
                result = l + 2 * c * n;
            }
        }
        if (std::isnan(result.x) || std::isnan(result.y)) {
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

    double ColdPlasma::getRefractiveIndex(const Element &element) const {
        auto permittivity = this->getPermittivity(element);
        if (permittivity.real() < 0) return 0;
        auto root = std::sqrt(permittivity);
        if (std::isnan(root.real())) {
            throw std::logic_error("Nan index of refraction!");
        }
        return root.real();
    }

    std::complex<double>
    ColdPlasma::getPermittivity(const Element &element) const {
        using namespace std::complex_literals;

        auto nu_ei = collFreq ? collFreq->getValue(element) : 0;
        auto n_e = density.getValue(element);
        auto m_e = constants::electron_mass;
        auto e = constants::electron_charge;
        auto omega = 2 * M_PI * constants::speed_of_light / wavelength.asDouble;
        auto omega_p2 = 4 * M_PI * e * e * n_e / m_e;

        auto term = omega_p2 / (omega * omega + nu_ei * nu_ei);
        return {1 - term, nu_ei / omega * term};
    }

    double ColdPlasma::getInverseBremsstrahlungCoeff(const Element &element) const {
        auto eps = getPermittivity(element);
        return 4 * M_PI / wavelength.asDouble * std::sqrt(eps).imag();
    }

    ColdPlasma::ColdPlasma(const MeshFunc &density, const Length &wavelength, const MeshFunc *collFreq) :
            density(density), collFreq(collFreq), wavelength(wavelength) {}

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
    ContinueStraight::operator()(const PointOnFace &, const Vector &previousDirection, const Element &,
                                 const Element &) {
        return previousDirection;
    }
}