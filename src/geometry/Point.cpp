#include "geometry/Point.h"

raytracer::geometry::Point::Point(double x, double y) : x(x), y(y) {}

const raytracer::geometry::Vector raytracer::geometry::operator-(raytracer::geometry::Point A, raytracer::geometry::Point B) {
    return {A.x - B.x, A.y - B.y};
}

const raytracer::geometry::Point
raytracer::geometry::operator+(raytracer::geometry::Point A, raytracer::geometry::Vector b) {
    return {A.x + b.x, A.y + b.y};
}

const raytracer::geometry::Point
raytracer::geometry::operator+(raytracer::geometry::Vector b, raytracer::geometry::Point A) {
    return A + b;
}
