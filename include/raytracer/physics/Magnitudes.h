#ifndef RAYTRACER_MAGNITUDES_H
#define RAYTRACER_MAGNITUDES_H

namespace raytracer {
    namespace physics {
        /** Strong type representing energy in joules. */
        struct Energy {double asDouble;};
        /** Strong type representing length in cm. */
        struct Length {double asDouble;};
        /** Strong type representing length in cm^-3. */
        struct Density {double asDouble;};
        /** Strong type representing temperature in eV. */
        struct Temperature {double asDouble;};
        /** Strong type representing frequency in s^-1. */
        struct Frequency {double asDouble;};
    }
}

#endif //RAYTRACER_MAGNITUDES_H
