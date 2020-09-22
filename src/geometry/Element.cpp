#include <set>
#include "Element.h"

namespace raytracer {
    int Element::getId() const {
        return this->id;
    }

    Element::Element(int id, std::vector<Face *> faces) :
            id(id),
            faces(std::move(faces)) {
        for (const auto& face : this->faces){
            this->points.emplace_back(face->getPoints()[0]);
        }
    }

    const std::vector<Face *> &Element::getFaces() const {
        return this->faces;
    }
}
