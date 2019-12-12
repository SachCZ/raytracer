#ifndef RAYTRACER_CONSTANTS_GEOMETRY_H
#define RAYTRACER_CONSTANTS_GEOMETRY_H

#include <limits>

namespace raytracer {
    namespace geometry {

        /**
         * Namespace encapsulating geometry constants
         */
        namespace constants {
            /**
             * Tolerance up to which two doubles are considered equal
             */
            constexpr auto epsilon = 100 * std::numeric_limits<double>::epsilon();
        }
    }
}

#endif //RAYTRACER_CONSTANTS_GEOMETRY_H
