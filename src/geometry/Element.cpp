#include <set>
#include "Element.h"

namespace raytracer {
    int Element::getId() const {
        return this->id;
    }

    Element::Element(int id, std::vector<Face *> faces) :
            id(id),
            faces(std::move(faces)) {
        std::set<Point*> uniquePoints;
        for (const auto& face : this->faces){
            for (const auto& point : face->getPoints()){
                uniquePoints.insert(point);
            }
        }
        this->points.assign(uniquePoints.begin(), uniquePoints.end());
    }

    const std::vector<Face *> &Element::getFaces() const {
        return this->faces;
    }
}
