#include "Termination.h"

namespace raytracer {

    StopAtCritical::StopAtCritical(const MeshFunction &density) :
            density(density) {}

    bool StopAtCritical::operator()(const Element &element, const LaserRay &laserRay) {
        auto currentDensity = density.getValue(element);
        auto criticalDensity = laserRay.getCriticalDensity();
        return currentDensity > criticalDensity.asDouble;
    }

    bool DontStop::operator()(const Element &, const LaserRay &) {
        return false;
    }
    
}