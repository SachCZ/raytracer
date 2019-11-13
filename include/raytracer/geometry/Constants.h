#ifndef RAYTRACER_CONSTANTS_H
#define RAYTRACER_CONSTANTS_H

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
            constexpr auto epsilon = std::numeric_limits<double>::epsilon();
        }
    }
}

#endif //RAYTRACER_CONSTANTS_H
