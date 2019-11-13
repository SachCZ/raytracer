#ifndef RAYTRACER_POINT_H
#define RAYTRACER_POINT_H

#include <cmath>

#include "Vector.h"

namespace raytracer {
    namespace geometry {

        /**
         * Class representing a point
         * A point is given by two coordinates x and y
         */
        class Point {
        public:
            /**
             * Construct the edge fiven x and y.
             * Note that x goes first, be sure not to mix this up.
             * @param x coordinate
             * @param y coordinate
             */
            Point(double x, double y);

            /**
             * Default contructor.
             * Default contructed point has both x and y equal to 0
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

            /**
             * Equality operator.
             * Two points are equal if their coordinates difference is less than given
             * project wide epsilon (defined in constants)
             * @param anotherPoint to comapre with
             * @return true if they almost equal
             */
            bool operator==(const Point& anotherPoint) const;

        private:
            friend const Vector operator - (Point A, Point B);
            friend const Point operator + (Point A, Vector b);
            friend const Point operator + (Vector b, Point A);
        };

        /**
         * Two Points can be subtracted to get a Vector
         * @param A point
         * @param B point
         * @return vector given by points difference
         */
        const Vector operator - (Point A, Point B);

        /**
         * Vector and point can be added get another point.
         * @param A point
         * @param b vector
         * @return Point given by vector added to point A
         */
        const Point operator + (Point A, Vector b);
        /**
         * Convenience wrapper around A + b
         * @param b
         * @param A
         * @return
         */
        const Point operator + (Vector b, Point A);

    }
}


#endif //RAYTRACER_POINT_H
