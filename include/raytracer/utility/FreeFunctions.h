#ifndef RAYTRACER_FREE_FUNCTIONS_H
#define RAYTRACER_FREE_FUNCTIONS_H

#include <vector>

namespace raytracer {
    /**
     * Return a vector of N equally distanced values from a to b.
     * @tparam T
     * @param a
     * @param b
     * @param N
     * @return
     */
    template<typename T>
    std::vector<T> linspace(T a, T b, size_t N) {
        T h = (b - a) / static_cast<T>(N - 1);
        std::vector<T> xs(N);
        typename std::vector<T>::iterator x;
        T val;
        for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
            *x = val;
        return xs;
    }

    /**
     * Given a function, integrate it at a point x using central rectangle approximation.
     * @tparam T
     * @tparam Function
     * @param function
     * @param x
     * @param dx
     * @return
     */
    template<typename T, typename Function>
    T integrateTrapz(Function function, T x, T dx) {
        return dx * (function(x) + function(x + dx)) / 2.0;
    }

    /**
     * Class representing a 1D gaussian with given parametrs
     */
    class Gaussian {
    public:
        /**
         * Construct the gaussian using FWHM, value at maxim (also the value of the integral) and the center point
         * @param FWHM
         * @param normalization or also the intgral value
         * @param center point
         */
        explicit Gaussian(double FWHM, double normalization = 1, double center = 0);

        /**
         * Value of the gaussian at given point x
         * @param x
         * @return the value
         */
        double operator()(double x);

    private:
        double FWHM;
        double normalization;
        double center;
    };
    
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
