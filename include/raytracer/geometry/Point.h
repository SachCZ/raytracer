#ifndef RAYTRACER_POINT_H
#define RAYTRACER_POINT_H

#include "Vector.h"
#include <cmath>
#include "Constants.h"

namespace raytracer {
    namespace geometry {

        class Point {
        public:
            Point(double x, double y);
            Point() = default;

            double x;
            double y;

            bool operator==(const Point& anotherPoint) const {
                return std::abs(this->x - anotherPoint.x) < constants::epsilon &&
                       std::abs(this->y - anotherPoint.y) < constants::epsilon;
            }

            friend const Vector operator - (Point A, Point B);
            friend const Point operator + (Point A, Vector b);
            friend const Point operator + (Vector b, Point A);
        };

        const Vector operator - (Point A, Point B);
        const Point operator + (Point A, Vector b);
        const Point operator + (Vector b, Point A);

    }
}


#endif //RAYTRACER_POINT_H
