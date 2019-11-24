#include <functional>
#include "MeshFunction.h"

raytracer::impl::MeshFunctionGe raytracer::geometry::MeshFunction::greaterOrEqual(
        double threshold,
        const std::function<void(raytracer::geometry::Quadrilateral)> &callback) {
    return {*this, threshold, callback};
}
