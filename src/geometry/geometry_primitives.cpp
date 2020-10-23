#include <cmath>
#include <set>
#include <utility>
#include "geometry_primitives.h"

namespace raytracer {
    Point::Point(const Vector &vector) : x(vector.x), y(vector.y) {}

    Point::Point(double x, double y) : x(x), y(y) {}

    Vector operator-(Point A, Point B) {
        return {A.x - B.x, A.y - B.y};
    }

    std::ostream &operator<<(std::ostream &os, const Point &point) {
        os << "(" <<point.x << ", " << point.y << ')';
        return os;
    }

    Vector operator*(double k, Vector a) {
        return {k * a.x, k * a.y};
    }

    Vector operator*(Vector a, double k) {
        return k * a;
    }

    Vector operator+(Vector a, Vector b) {
        return {a.x + b.x, a.y + b.y};
    }

    Vector operator-(Vector a, Vector b) {
        return a + (-1.0 * b);
    }

    double operator*(Vector a, Vector b) {
        return a.x * b.x + a.y * b.y;
    }

    std::ostream &operator<<(std::ostream &os, const Vector &vector) {
        os << "(" <<vector.x << ", " << vector.y << ')';
        return os;
    }

    double Vector::getNorm() const {
        return std::sqrt(this->getNorm2());
    }

    double Vector::getNorm2() const {
        return x*x + y*y;
    }

    Vector::Vector(const Point &point) : x(point.x), y(point.y) {}

    Vector::Vector(double x, double y) : x(x), y(y) {}

    Vector Face::getNormal() const {
        if (this->points.size() == 2) {
            auto direction = *points[1] - *points[0];
            return direction.getNormal();
        } else {
            throw std::logic_error("Can get normal to face!");
        }
    }

    const std::vector<Point *> &Face::getPoints() const {
        return this->points;
    }

    Face::Face(int id, std::vector<Point *> points) :
            id(id),
            points(std::move(points)) {}

    int Face::getId() const {
        return this->id;
    }

    int Element::getId() const {
        return this->id;
    }

    Element::Element(int id, std::vector<Face *> faces, std::vector<Point *> points) :
            id(id),
            faces(std::move(faces)), points(std::move(points)) {}

    const std::vector<Face *> &Element::getFaces() const {
        return this->faces;
    }

    const std::vector<Point *> &Element::getPoints() const {
        return this->points;
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

