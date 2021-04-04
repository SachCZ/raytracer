#include "numeric.h"
#include <cmath>
#include <limits>

namespace raytracer {
    IntNormGaussian::IntNormGaussian(double FWHM, double normalization, double center) :
            FWHM(FWHM), normalization(normalization), center(center) {}

    double IntNormGaussian::operator()(double x) {
        auto w = this->FWHM;
        auto a = this->normalization * (2 * std::sqrt(std::log(2))) / (w * std::sqrt(M_PI));
        auto b = this->center;
        return a * std::exp(-4 * std::log(2) * std::pow(x - b, 2) / std::pow(w, 2));
    }

    MaxValGaussian::MaxValGaussian(double FWHM, double maxValue, double center)
            : sigma(FWHM / 2.355), maxValue(maxValue), center(center) {}

    double MaxValGaussian::operator()(double x) const {
        return maxValue * std::exp(-std::pow(x - center, 2) / 2 / std::pow(sigma, 2));
    }
}