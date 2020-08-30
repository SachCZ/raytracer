#include <cmath>
#include <iostream>
#include "Vector.h"

namespace raytracer {
    Vector::Vector(double x, double y) : x(x), y(y) {}

    const Vector operator*(double k, Vector a) {
        return {k * a.x, k * a.y};
    }

    const Vector operator*(Vector a, double k) {
        return k * a;
    }

    const Vector operator+(Vector a, Vector b) {
        return {a.x + b.x, a.y + b.y};
    }

    const Vector operator-(Vector a, Vector b) {
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
        return std::sqrt(x * x + y * y);
    }
}