#ifndef RAYTRACER_PROPAGATION_H
#define RAYTRACER_PROPAGATION_H

#include "mesh_function.h"
#include "gradient.h"
#include "collisional_frequency.h"

namespace raytracer {
    /**
     * \addtogroup intersectionFinders
     * @{
     */

    /**
     * Function to be used in Laser::generateIntersections() as findIntersection.
     * It intersects the element in straight line.
     * @param entryPointOnFace
     * @param entryDirection
     * @param element
     * @return intersecting point in straight line from entryPointOnFace
     */
    PointOnFace intersectStraight(
            const PointOnFace &entryPointOnFace,
            const Vector &entryDirection,
            const Element &element
    );

    /**
     * @}
     */
}


#endif //RAYTRACER_PROPAGATION_H
