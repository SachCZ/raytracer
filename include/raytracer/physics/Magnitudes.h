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
    }
}

#endif //RAYTRACER_MAGNITUDES_H
