#ifndef RAYTRACER_COLLISIONALFREQUENCY_H
#define RAYTRACER_COLLISIONALFREQUENCY_H

#include "Magnitudes.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

namespace raytracer {
    /**
     * \addtogroup frequency
     * @{
     */

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

    /**
     * @}
     */
}


#endif //RAYTRACER_COLLISIONALFREQUENCY_H
