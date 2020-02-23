#include "LaserRay.h"
#include "Constants.h"

namespace raytracer {
    namespace physics {
        Density LaserRay::getCriticalDensity() const {
            auto m_e = constants::electron_mass;
            auto c = constants::speed_of_light;
            auto e = constants::electron_charge;
            auto lamb = this->wavelength;

            auto constant = m_e * M_PI * std::pow(c, 2) / std::pow(e, 2);

            return { constant *  std::pow(lamb.asDouble, -2)};
        }

        double LaserRay::getRefractiveIndex(const Density &density, const Frequency& collisionFrequency) const {
            return std::sqrt(this->getPermittivity(density, collisionFrequency)).real();
        }
    }
}

