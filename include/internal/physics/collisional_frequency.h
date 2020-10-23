#ifndef RAYTRACER_COLLISIONAL_FREQUENCY_H
#define RAYTRACER_COLLISIONAL_FREQUENCY_H

#include "magnitudes.h"
#include <cmath>
#include <algorithm>
#include <geometry.h>

namespace raytracer {
    double calcSpitzerFreq(double dens, double temp, double ioni, Length wavelen);

    namespace impl {
        double calcCoulombLog(double dens, double temp, double ioni, Length wavelen);
    }
}


#endif //RAYTRACER_COLLISIONAL_FREQUENCY_H
