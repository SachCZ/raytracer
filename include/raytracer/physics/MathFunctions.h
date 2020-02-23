#ifndef RAYTRACER_MATH_FUNCTIONS_H
#define RAYTRACER_MATH_FUNCTIONS_H

#include <cmath>
#include <Vector.h>

namespace raytracer {
    namespace physics {

        /**
         * Class representing a 1D gaussian with given parametrs
         */
        class Gaussian {
        public:
            /**
             * Construct the gaussian using FWHM, value at maxim (also the value of the integral) and the center point
             * @param FWHM
             * @param maximum or also the intgral value
             * @param center point
             */
            explicit Gaussian(double FWHM, double maximum = 1, double center = 0);

            /**
             * Value of the gaussian at given point x
             * @param x
             * @return the value
             */
            double operator()(double x);

        private:
            double FWHM;
            double maximum;
            double center;
        };
    }
}

#endif //RAYTRACER_MATH_FUNCTIONS_H
