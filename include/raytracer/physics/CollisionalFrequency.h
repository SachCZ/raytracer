#ifndef RAYTRACER_COLLISIONALFREQUENCY_H
#define RAYTRACER_COLLISIONALFREQUENCY_H

#include "Magnitudes.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

namespace raytracer {

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
        ) const override;

    private:
        double getCoulombLogarithm(
                const Density &density,
                const Temperature &temperature,
                const Length &laserWavelength,
                double ionization
        ) const;
    };

}


#endif //RAYTRACER_COLLISIONALFREQUENCY_H
