#ifndef RAYTRACER_PROPAGATION_H
#define RAYTRACER_PROPAGATION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Gradient.h"
#include "CollisionalFrequency.h"

namespace raytracer {
    namespace physics {


        geometry::PointOnFace intersectStraight(
                const geometry::PointOnFace &pointOnFace,
                const geometry::Vector &direction,
                const geometry::Element &nextElement,
                const LaserRay &
        );
    }
}


#endif //RAYTRACER_PROPAGATION_H
