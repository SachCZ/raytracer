#include "numeric.h"
#include <cmath>
#include <limits>

namespace raytracer {
    Gaussian::Gaussian(double FWHM, double normalization, double center) :
            FWHM(FWHM), normalization(normalization), center(center) {}

    double Gaussian::operator()(double x) {
        auto w = this->FWHM;
        auto a = this->normalization * (2 * std::sqrt(std::log(2))) / (w * std::sqrt(M_PI));
        auto b = this->center;
        return a * std::exp(-4 * std::log(2) * std::pow(x - b, 2) / std::pow(w, 2));
    }
}