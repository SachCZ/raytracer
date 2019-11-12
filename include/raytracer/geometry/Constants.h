#ifndef RAYTRACER_CONSTANTS_H
#define RAYTRACER_CONSTANTS_H

#include <limits>

namespace raytracer {
    namespace geometry {
        namespace constants {
            constexpr double epsilon = std::numeric_limits<double>::epsilon();
        }
    }
}

#endif //RAYTRACER_CONSTANTS_H
