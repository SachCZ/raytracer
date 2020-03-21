#ifndef RAYTRACER_POINT_H
#define RAYTRACER_POINT_H

#include "Vector.h"

namespace raytracer {

    /**
     * Class representing a point
     * A point is given by two coordinates x and y
     */
    class Point {
    public:
        /**
         * Construct the edge given x and y.
         * Note that x goes first, be sure not to mix this up.
         * @param x coordinate
         * @param y coordinate
         */
        Point(double x, double y);

        /**
         * Default constructor.
         * Default constructed point has both x and y equal to 0
         */
        Point() = default;

        /**
         * x coordinate
         */
        double x;
        /**
         * y coordinate
         */
        double y;
    };

    /**
     * Two Points can be subtracted to get a Vector
     * @param A point
     * @param B point
     * @return vector given by points difference
     */
    const Vector operator-(Point A, Point B);

    /**
     * Vector and point can be added get another point.
     * @param A point
     * @param b vector
     * @return Point given by vector added to point A
     */
    const Point operator+(Point A, Vector b);

    /**
     * Convenience wrapper around A + b
     * @param b
     * @param A
     * @return
     */
    const Point operator+(Vector b, Point A);


}


#endif //RAYTRACER_POINT_H
