#ifndef RAYTRACER_VECTOR_H
#define RAYTRACER_VECTOR_H

namespace raytracer {
    namespace geometry {
        /**
         * Class representing a physical vector
         */
        class Vector {
        public:
            /**
             * Construct the vector using its coordinates.
             * @param x coordinate
             * @param y coordinate
             */
            Vector(double x, double y);

            /**
             * Default constructor initializes a (0, 0) vector.
             */
            Vector() = default;

            /**
             * x coordinate
             */
            double x;
            /**
             * y coordinate
             */
            double y;
            /**
             * Return the Euclidean norm of the vector (square root of sum of coordinates squared)
             * @return size of the vector
             */
            double getNorm() const;

            /**
             * Return the normal to the vector using the convention (y, -x)
             * @return
             */
            Vector getNormal() const {
                return {this->y, -this->x};
            }
        };

        /**
         * Vector can be multiplied by number k*a
         * @param k
         * @param a
         * @return scaled vector
         */
        const Vector operator * (double k, Vector a);

        /**
         * Convenience wrapper around k*a
         * @param a
         * @param k
         * @return
         */
        const Vector operator * (Vector a, double k);

        /**
         * his is the dot product of two vectors a, b
         * @param a
         * @param b
         * @return dot product
         */
        double operator * (Vector a, Vector b);

        /**
         * Two vectors a, b can be added
         * @param a
         * @param b
         * @return
         */
        const Vector operator + (Vector a, Vector b);

        /**
         * Two vectors a, b can be subtracted
         * @param a
         * @param b
         * @return
         */
        const Vector operator - (Vector a, Vector b);
    }
}


#endif //RAYTRACER_VECTOR_H
