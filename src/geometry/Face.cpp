#include "Face.h"

namespace raytracer {
    namespace geometry {
        Vector Face::getNormal() const {
            if (this->points.size() == 2){
                auto direction = *points[1] - *points[0];
                return direction.getNormal();
            } else {
                throw std::logic_error("Can get normal to face!");
            }
        }

        const std::vector<Point*>& Face::getPoints() const {
            return this->points;
        }

        Face::Face(int id, std::vector<Point*> points) :
                id(id),
                points(std::move(points)) {}

        const Point *Face::isBoundary(const Point &point) const {
            for (const auto& referencePoint : this->points) {
                if ((point - *referencePoint).getNorm() < constants::epsilon) return referencePoint;
            }
            return nullptr;
        }
    }
}