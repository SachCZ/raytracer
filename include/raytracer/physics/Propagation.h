#ifndef RAYTRACER_PROPAGATION_H
#define RAYTRACER_PROPAGATION_H

#include "MeshFunction.h"
#include "LaserRay.h"
#include "Gradient.h"
#include "CollisionalFrequency.h"

namespace raytracer {


    PointOnFace intersectStraight(
            const PointOnFace &pointOnFace,
            const Vector &direction,
            const Element &nextElement,
            const LaserRay &
    );

}


#endif //RAYTRACER_PROPAGATION_H
