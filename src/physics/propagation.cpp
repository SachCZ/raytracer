#include "propagation.h"
#include "refraction.h"
#include "termination.h"

namespace raytracer {
    PointOnFace intersectStraight(
            const PointOnFace &entryPointOnFace,
            const Vector &entryDirection,
            const Element &element
    ) {
        const auto &faces = element.getFaces();
        std::vector<Face *> facesToSearch;
        facesToSearch.reserve(faces.size());
        std::copy_if(faces.begin(), faces.end(), std::back_inserter(facesToSearch),
                     [&entryPointOnFace](const auto &face) {
                         return entryPointOnFace.face != face;
                     });
        auto newPointOnFace = findClosestIntersectionPoint(
                {entryPointOnFace.point, entryDirection},
                facesToSearch
        );
        if (!newPointOnFace) throw std::logic_error("No intersection found, but it should definitely exist!");
        return *newPointOnFace;
    }

}

