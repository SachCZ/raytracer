#include "Element.h"

namespace raytracer {
    namespace geometry {
        const Face* Element::getBoundaryPointFirstAdjacentFace(const Point* point, const Face* omitFace) const {
            for (const auto& referenceFace : this->getFaces()){
                for (const auto& referencePoint : referenceFace->getPoints()){
                    if (referenceFace != omitFace && referencePoint == point){
                        return referenceFace;
                    }
                }
            }
            return nullptr;
        }
    }
}
