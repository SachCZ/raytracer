#ifndef RAYTRACER_PROPAGATION_H
#define RAYTRACER_PROPAGATION_H

#include <geometry.h>

namespace raytracer {
    /**
     * Given an element and entry ray calculate how the element is intersected and return the resulting point
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
}


#endif //RAYTRACER_PROPAGATION_H
