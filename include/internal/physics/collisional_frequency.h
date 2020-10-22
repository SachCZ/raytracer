#ifndef RAYTRACER_COLLISIONAL_FREQUENCY_H
#define RAYTRACER_COLLISIONAL_FREQUENCY_H

#include "magnitudes.h"
#include <cmath>
#include <algorithm>
#include <geometry.h>

namespace raytracer {
    /**
     * Calculate the spitzer frequency in each element
     * @param dens
     * @param temp
     * @param ioni
     * @param wavelen
     * @return
     */
    MeshFunc::Ptr calcSpitzerFreq(
            const MeshFunc &dens,
            const MeshFunc &temp,
            const MeshFunc &ioni,
            const Length &wavelen
    );

    namespace impl {
        double calcSpitzerFreq(double dens, double temp, double ioni, Length wavelen);
        double calcCoulombLog(double dens, double temp, double ioni, Length wavelen);
    }
}


#endif //RAYTRACER_COLLISIONAL_FREQUENCY_H
