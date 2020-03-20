#ifndef RAYTRACER_COLLISIONALFREQUENCY_H
#define RAYTRACER_COLLISIONALFREQUENCY_H

#include "Magnitudes.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

namespace raytracer {
    namespace physics {
        class CollisionalFrequencyCalculator {
        public:
            virtual Frequency getCollisionalFrequency(
                    const Density &density,
                    const Temperature &temperature,
                    const Length &laserWavelength,
                    double ionization
            ) const = 0;
        };

        class SpitzerFrequencyCalculator : public CollisionalFrequencyCalculator {
        public:
            Frequency getCollisionalFrequency(
                    const Density &density,
                    const Temperature &temperature,
                    const Length &laserWavelength,
                    double ionization
            ) const override {
                auto n_e = density.asDouble;
                auto T_e = temperature.asDouble;
                auto Z = ionization;
                auto e = constants::electron_charge;
                auto m_e = constants::electron_mass;
                auto k_b = constants::boltzmann_constant;
                auto ln_lamb = this->getCoulombLogarithm(density, temperature, laserWavelength, ionization);
                auto h = constants::reduced_planck_constant;
                auto E_F = h*h / (2*m_e) * std::pow(3*M_PI*M_PI*n_e, 2.0/3.0);

                return {4.0 / 3.0 * std::sqrt(2 * M_PI) * Z * std::pow(e, 4) * n_e / std::sqrt(m_e) /
                        std::pow(k_b * T_e + E_F, 3.0 / 2.0) * ln_lamb};
            }

        private:
            double getCoulombLogarithm(
                    const Density &density,
                    const Temperature &temperature,
                    const Length &laserWavelength,
                    double ionization
            ) const {
                auto e = constants::electron_charge;
                auto m_e = constants::electron_mass;
                auto k_b = constants::boltzmann_constant;
                auto Z = ionization;
                auto h = constants::reduced_planck_constant;
                auto n_e = density.asDouble;
                auto T_e = temperature.asDouble;
                auto omega = 2 * M_PI * constants::speed_of_light / laserWavelength.asDouble;
                auto omega_p = std::sqrt(4 * M_PI * e * e * n_e / m_e);

                auto b_max = std::sqrt(k_b * T_e / m_e) / std::max(omega, omega_p);
                auto b_min = std::max(Z * e * e / k_b / T_e, h / std::sqrt(k_b * T_e / m_e));

                return std::max(2.0, 0.5 * std::log(std::abs(b_max) / std::abs(b_min)));
            }
        };
    }
}


#endif //RAYTRACER_COLLISIONALFREQUENCY_H
