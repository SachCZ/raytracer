#include "termination.h"

namespace raytracer {

    StopAtDensity::StopAtDensity(const MeshFunc &density, Density stopAt) :
            density(density), stopAt(stopAt) {}

    bool StopAtDensity::operator()(const Element &element) {
        auto currentDensity = density.getValue(element);
        return currentDensity > stopAt.asDouble;
    }

    bool dontStop(const Element &) {return false;}
}