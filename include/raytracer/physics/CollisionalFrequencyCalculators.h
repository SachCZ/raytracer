#ifndef RAYTRACER_COLLISIONALFREQUENCYCALCULATORS_H
#define RAYTRACER_COLLISIONALFREQUENCYCALCULATORS_H

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
                auto ln_lamb = this->getCoulombLogarithm();

                return {4.0 / 3.0 * std::sqrt(2 * M_PI) * Z * std::pow(e, 4) * m_e * n_e /
                        std::pow(m_e * k_b * T_e, 3.0 / 2.0) * ln_lamb};
            }

        private:
            double getCoulombLogarithm(
                    const Temperature &temperature,
                    double ionization,
                    const Length &laserWavelength
            ) const {
                auto omega =
                return std::max(2, 0.5 * std::log(std::abs(b_max) / std::abs(b_min)));
            }
        };
    }
}


#endif //RAYTRACER_COLLISIONALFREQUENCYCALCULATORS_H
