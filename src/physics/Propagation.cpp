#include "Propagation.h"
#include "Refraction.h"
#include "Termination.h"

namespace raytracer {
    namespace physics {
        geometry::PointOnFace
        intersectStraight(
                const geometry::PointOnFace &pointOnFace,
                const geometry::Vector &direction,
                const geometry::Element &nextElement,
                const LaserRay &
        ) {
            auto newPointOnFace = geometry::findClosestIntersection(
                    {pointOnFace.point, direction},
                    nextElement.getFaces(),
                    pointOnFace.face
            );
            if (!newPointOnFace) throw std::logic_error("No intersection found, but it should definitely exist!");
            return *newPointOnFace;
        }
    }
}

