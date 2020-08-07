#include "Propagation.h"
#include "Refraction.h"
#include "Termination.h"

namespace raytracer {
    PointOnFace
    intersectStraight(
            const PointOnFace &entryPointOnFace,
            const Vector &entryDirection,
            const Element &element
    ) {
        const auto &faces = element.getFaces();
        std::vector<Face *> facesToSearch;
        std::copy_if(faces.begin(), faces.end(), std::back_inserter(facesToSearch),
                     [&entryPointOnFace](const auto &face) {
                         return entryPointOnFace.face != face;
                     });
        auto newPointOnFace = findClosestIntersection(
                {entryPointOnFace.point, entryDirection},
                facesToSearch
        );
        if (!newPointOnFace) throw std::logic_error("No intersection found, but it should definitely exist!");
        return *newPointOnFace;
    }

}

