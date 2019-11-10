#include "Vector.h"

raytracer::geometry::Vector::Vector(double x, double y) : x(x), y(y) {}

const raytracer::geometry::Vector raytracer::geometry::operator*(double k, raytracer::geometry::Vector a) {
    return {k*a.x, k*a.y};
}

const raytracer::geometry::Vector
raytracer::geometry::operator*(raytracer::geometry::Vector a, double k) {
    return k*a;
}

const raytracer::geometry::Vector
raytracer::geometry::operator+(raytracer::geometry::Vector a, raytracer::geometry::Vector b) {
    return {a.x + b.x, a.y + b.y};
}

const raytracer::geometry::Vector
raytracer::geometry::operator-(raytracer::geometry::Vector a, raytracer::geometry::Vector b) {
    return a + (-1.0 * b);
}

double raytracer::geometry::operator*(raytracer::geometry::Vector a, raytracer::geometry::Vector b) {
    return a.x * b.x + a.y * b.y;
}
