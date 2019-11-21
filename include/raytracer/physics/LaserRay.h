#ifndef RAYTRACER_LASER_RAY_H
#define RAYTRACER_LASER_RAY_H

#include <cmath>

#include "Magnitudes.h"
#include "raytracer/geometry/Point.h"
#include "raytracer/geometry/Vector.h"
#include "Constants.h"

namespace raytracer {
    namespace physics {
        struct LaserRay {
            geometry::Point startPoint{};
            geometry::Vector direction{};
            Energy energy{};
            Length wavelength{};

            Density getCriticalDensity() const;
        };
    }
}

#endif //RAYTRACER_LASER_RAY_H
