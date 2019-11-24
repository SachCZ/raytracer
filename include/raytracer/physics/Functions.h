#ifndef RAYTRACER_FUNCTIONS_H
#define RAYTRACER_FUNCTIONS_H

#include <cmath>
#include <Vector.h>

namespace raytracer {
    namespace physics {
        class Gaussian {
        public:
            explicit Gaussian(double FWHM, double maximum = 1, double center = 0):
            FWHM(FWHM), maximum(maximum), center(center) {}

            double operator()(double x){
                auto w = this->FWHM;
                auto a = this->maximum;
                auto b = this->center;
                return a * std::exp(-4 * std::log(2) * std::pow(x - b, 2) / std::pow(w, 2));
            }

        private:
            double FWHM;
            double maximum;
            double center;
        };

        class LinearFunction {
        public:
            LinearFunction(double k, double q):
            k(k), q(q) {}

            double operator()(double x){
                return k*x + q;
            }

        private:
            double k, q;
        };

        
    }
}

#endif //RAYTRACER_FUNCTIONS_H
