#include "Propagation.h"
#include "Refraction.h"
#include "Termination.h"

namespace raytracer {
    PointOnFace
    intersectStraight(
            const PointOnFace &entryPointOnFace,
            const Vector &entryDirection,
            const Element &element,
            const LaserRay &
    ) {
        auto newPointOnFace = findClosestIntersection(
                {entryPointOnFace.point, entryDirection},
                element.getFaces(),
                entryPointOnFace.face
        );
        if (!newPointOnFace) throw std::logic_error("No intersection found, but it should definitely exist!");
        return *newPointOnFace;
    }
    
}

