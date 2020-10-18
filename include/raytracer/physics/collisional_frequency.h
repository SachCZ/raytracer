#ifndef RAYTRACER_COLLISIONAL_FREQUENCY_H
#define RAYTRACER_COLLISIONAL_FREQUENCY_H

#include "magnitudes.h"
#include <cmath>
#include <algorithm>

namespace raytracer {
    /**
     * Abstract interface. To obey this interface a method get returning the collisional frequency must be implemented.
     */
    class CollisionalFrequency {
    public:
        /**
         * Override this.
         * @param density
         * @param temperature
         * @param laserWavelength
         * @param ionization
         * @return collisional frequency given state variables
         */
        virtual Frequency get(
                const Density &density,
                const Temperature &temperature,
                const Length &laserWavelength,
                double ionization
        ) const = 0;
    };

    /**
     * Class representing a Spitzer-Harm frequency calculator.
     */
    class SpitzerFrequency : public CollisionalFrequency {
    public:
        /**
         * Based on the state variables return the Spitzer Harm frequency obtained according to Velechovsky thesis.
         * @param density
         * @param temperature
         * @param laserWavelength
         * @param ionization
         * @return the collisional frequency
         */
        Frequency get(
                const Density &density,
                const Temperature &temperature,
                const Length &laserWavelength,
                double ionization
        ) const override;

    private:
        double getCoulombLogarithm(
                const Density &density,
                const Temperature &temperature,
                const Length &laserWavelength,
                double ionization
        ) const;
    };

    class ConstantFrequency : public CollisionalFrequency {
    public:
        explicit ConstantFrequency(const Frequency& frequency);

        Frequency get(const Density &density, const Temperature &temperature, const Length &laserWavelength,
                      double ionization) const override;

    private:
        Frequency frequency;
    };
}


#endif //RAYTRACER_COLLISIONAL_FREQUENCY_H
