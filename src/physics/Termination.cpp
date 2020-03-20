#include "Termination.h"

namespace raytracer {
    namespace physics {
        StopAtCritical::StopAtCritical(const geometry::MeshFunction &density) :
                density(density) {}

        bool StopAtCritical::operator()(const geometry::Element &element, const LaserRay &laserRay) {
            auto currentDensity = density.getValue(element);
            auto criticalDensity = laserRay.getCriticalDensity();
            return currentDensity > criticalDensity.asDouble;
        }

        bool DontStop::operator()(const geometry::Element &, const LaserRay &) {
            return false;
        }
    }
}