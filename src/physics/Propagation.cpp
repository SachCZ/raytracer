#include "Propagation.h"
#include "Refraction.h"
#include "Termination.h"

namespace raytracer {
    PointOnFace
    intersectStraight(
            const PointOnFace &pointOnFace,
            const Vector &direction,
            const Element &nextElement,
            const LaserRay &
    ) {
        auto newPointOnFace = findClosestIntersection(
                {pointOnFace.point, direction},
                nextElement.getFaces(),
                pointOnFace.face
        );
        if (!newPointOnFace) throw std::logic_error("No intersection found, but it should definitely exist!");
        return *newPointOnFace;
    }
    
}

