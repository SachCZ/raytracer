#include <stdexcept>
#include "collisional_frequency.h"
#include <cmath>
#include <geometry_primitives.h>
#include "constants.h"
#include <boost/math/special_functions/pow.hpp>

namespace raytracer {
    double calcSpitzerFreq(double dens, double temp, double ioni, Length wavelen) {
        using namespace boost::math;

        auto n_e = dens;
        auto T_e = temp;
        auto Z = ioni;

        auto e = constants::electron_charge;
        auto m_e = constants::electron_mass;
        auto k_b = constants::boltzmann_constant;
        auto ln_lamb = impl::calcCoulombLog(n_e, T_e, Z, wavelen);
        auto h = constants::reduced_planck_constant;
        auto E_F = h * h / (2 * m_e) * std::cbrt(pow<2>(3 * M_PI * M_PI * n_e));

        auto result = 4.0 / 3.0 * std::sqrt(2 * M_PI) * Z * pow<4>(e) * n_e / std::sqrt(m_e) /
                      std::sqrt(pow<3>(k_b * T_e + E_F)) * ln_lamb;
        if (std::isnan(result)) {
            throw std::logic_error("Nan collisional frequency!");
        }
        return result;
    }

    namespace impl {
        double calcCoulombLog(double dens, double temp, double ioni, Length wavelen) {
            auto n_e = dens;
            auto T_e = temp;
            auto Z = ioni;

            auto e = constants::electron_charge;
            auto m_e = constants::electron_mass;
            auto k_b = constants::boltzmann_constant;
            auto h = constants::reduced_planck_constant;

            auto omega = 2 * M_PI * constants::speed_of_light / wavelen.asDouble;
            auto omega_p = std::sqrt(4 * M_PI * e * e * n_e / m_e);

            auto b_max = std::sqrt(k_b * T_e / m_e) / std::max(omega, omega_p);
            auto b_min = std::max(Z * e * e / k_b / T_e, h / std::sqrt(k_b * T_e / m_e));

            return std::max(2.0, 0.5 * std::log(std::abs(b_max) / std::abs(b_min)));
        }
    }
}