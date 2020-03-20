#include <cmath>

#include "Point.h"

namespace raytracer {
    Point::Point(double x, double y) : x(x), y(y) {}

    const Vector operator-(Point A, Point B) {
        return {A.x - B.x, A.y - B.y};
    }

    const Point operator+(Point A, Vector b) {
        return {A.x + b.x, A.y + b.y};
    }

    const Point operator+(Vector b, Point A) {
        return A + b;
    }
}