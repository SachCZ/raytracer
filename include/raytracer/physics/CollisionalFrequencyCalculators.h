#ifndef RAYTRACER_COLLISIONALFREQUENCYCALCULATORS_H
#define RAYTRACER_COLLISIONALFREQUENCYCALCULATORS_H


#include "Magnitudes.h"

namespace raytracer {
    namespace physics {
        class CollisionalFrequencyCalculator {
        public:
            virtual Frequency getCollisionalFrequency(const Density& density, const Temperature& temperature) const = 0;
        };
    }
}


#endif //RAYTRACER_COLLISIONALFREQUENCYCALCULATORS_H
