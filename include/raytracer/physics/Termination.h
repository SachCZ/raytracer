#ifndef RAYTRACER_TERMINATION_H
#define RAYTRACER_TERMINATION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Gradient.h"
#include "CollisionalFrequency.h"

namespace raytracer {
    /**
     * \addtogroup stopConditions
     * @{
     */

    /**
    * Functor for ray propagation termination based on critical density expected to used with laseRay
    * intersection finding procedure
    */
    struct StopAtCritical {
        /**
         * Constructor the functor using a density MeshFunction
         * @param density
         */
        explicit StopAtCritical(const MeshFunction &density);

        /**
         * Returns true if the density at element to go to is greater than criticalDensity of
         * the LaserRay.
         *
         * @param element
         * @param laserRay
         * @return true if current density is grater than critical
         */
        bool operator()(const Element & element, const LaserRay &laserRay);

    private:
        const MeshFunction &density;
    };

    /**
     * Functor for ray propagation that returns false in any case. Convenience struct to keep the call similar to
     * eg. StopAtCritical.
     */
    struct DontStop {
        /**
         * Just returns false.
         * @return false.
         */
        bool operator()(const Element &, const LaserRay &);
    };

    /**
     * @}
     */
}

#endif //RAYTRACER_TERMINATION_H
