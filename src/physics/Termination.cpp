#include "Termination.h"

namespace raytracer {

    StopAtDensity::StopAtDensity(const MeshFunction &density, double stopAt) :
            density(density), stopAt(stopAt) {}

    bool StopAtDensity::operator()(const Element &element) {
        auto currentDensity = density.getValue(element);
        return currentDensity > stopAt;
    }

    bool DontStop::operator()(const Element &) {
        return false;
    }
    
}