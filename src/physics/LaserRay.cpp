#include "LaserRay.h"
#include "Constants.h"

namespace raytracer {

    Density LaserRay::getCriticalDensity() const {
        auto m_e = constants::electron_mass;
        auto c = constants::speed_of_light;
        auto e = constants::electron_charge;
        auto lamb = this->wavelength;

        auto constant = m_e * M_PI * std::pow(c, 2) / std::pow(e, 2);

        return {constant * std::pow(lamb.asDouble, -2)};
    }

    double LaserRay::getRefractiveIndex(const Density &density, const Frequency &collisionFrequency) const {
        auto permittivity = this->getPermittivity(density, collisionFrequency);
        if (permittivity.real() < 0) return 0;
        auto root = std::sqrt(permittivity);
        if (std::isnan(root.real())){
            throw std::logic_error("Nan index of refraction!");
        }
        return root.real();
    }

    std::complex<double> LaserRay::getPermittivity(const Density &density, const Frequency &collisionFrequency) const {
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

}

