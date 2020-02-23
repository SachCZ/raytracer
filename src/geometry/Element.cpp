#include "Element.h"

namespace raytracer {
    namespace geometry {
        int Element::getId() const {
            return this->id;
        }

        Element::Element(int id, std::vector<Face *> faces) :
                id(id),
                faces(std::move(faces)) {}

        const std::vector<Face *> &Element::getFaces() const {
            return this->faces;
        }
    }
}
