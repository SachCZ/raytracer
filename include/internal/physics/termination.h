#ifndef RAYTRACER_TERMINATION_H
#define RAYTRACER_TERMINATION_H

#include <geometry.h>
#include "gradient.h"
#include "collisional_frequency.h"

namespace raytracer {

    /**
    * Functor that returns true of a density condition is met
    */
    struct StopAtDensity {

        /**
         * Declare the density at which h to stop
         * @param density density to compare with threshold
         * @param stopAt threshold
         */
        explicit StopAtDensity(const MeshFunc &density, Density stopAt);

        /**
         * Returns true if the density at element to go to is greater than stopAt defined in constructor
         *
         * @param element
         * @return true if current density is grater than stopAt
         */
        bool operator()(const Element & element);

    private:
        const MeshFunc &density;
        const Density stopAt;
    };

    /**
     * Return false.
     * @return false.
     */
    bool dontStop(const Element &);

}

#endif //RAYTRACER_TERMINATION_H
