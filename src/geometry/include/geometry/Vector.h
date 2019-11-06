#ifndef RAYTRACER_VECTOR_H
#define RAYTRACER_VECTOR_H

#include <cmath>

namespace raytracer {
    namespace geometry {
        class Vector {
        public:
            Vector(double x, double y);

            double x;
            double y;
            double getNorm() const {
                return std::sqrt(x*x + y*y);
            }

            friend const Vector operator * (double k, Vector a);
            friend const Vector operator * (Vector a, double k);
            friend double operator * (Vector a, Vector b);
            friend const Vector operator + (Vector a, Vector b);
            friend const Vector operator - (Vector a, Vector b);
        };

        const Vector operator * (double k, Vector a);
        const Vector operator * (Vector a, double k);
        double operator * (Vector a, Vector b);
        const Vector operator + (Vector a, Vector b);
        const Vector operator - (Vector a, Vector b);
    }
}


#endif //RAYTRACER_VECTOR_H
