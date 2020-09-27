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

    Point getElementCentroid(const Element &element) {
        auto points = element.getPoints();
        if (points.size() == 3) {
            return {(points[0]->x + points[1]->x + points[2]->x) / 3.0,
                    (points[0]->y + points[1]->y + points[2]->y) / 3.0};
        } else {
            double sum_x = 0;
            double sum_y = 0;
            double sum_area = 0;
            for (uint i = 0; i < points.size(); i++){
                uint next = i == points.size() - 1 ? 0 : i + 1;
                auto xi = points[i]->x;
                auto xip1 = points[next]->x;
                auto yi = points[i]->y;
                auto yip1 = points[next]->y;
                sum_x += (xi + xip1)*(xi * yip1 - xip1*yi);
                sum_y += (yi + yip1)*(xi * yip1 - xip1*yi);
                sum_area += xi * yip1 - xip1*yi;
            }
            double signed_area = 1.0/2.0*sum_area;
            double centroid_x = 1.0 / 6.0 / signed_area * sum_x;
            double centroid_y = 1.0 / 6.0 / signed_area * sum_y;
            return {centroid_x, centroid_y};
        }
    }
}
