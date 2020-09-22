#include "FreeFunctions.h"
#include <cmath>

namespace raytracer {
    Gaussian::Gaussian(double FWHM, double normalization, double center) :
            FWHM(FWHM), normalization(normalization), center(center) {}

    double Gaussian::operator()(double x) {
        auto w = this->FWHM;
        auto a = this->normalization * (2 * std::sqrt(std::log(2))) / (w * std::sqrt(M_PI));
        auto b = this->center;
        return a * std::exp(-4 * std::log(2) * std::pow(x - b, 2) / std::pow(w, 2));
    }

    Point getPolygonCentroid(const Element &element) {
        auto points = element.getPoints();
        if (points.size() == 3) {
            return {(points[0]->x + points[1]->x + points[2]->x) / 3.0,
                    (points[0]->y + points[1]->y + points[2]->y) / 3.0};
        } else {
            double sum_x = 0;
            double sum_y = 0;
            double sum_area = 0;
            for (uint i = 0; i < points.size(); i++){
                uint next = i == points.size() - 1 ? 0 : i + 1;
                auto xi = points[i]->x;
                auto xip1 = points[next]->x;
                auto yi = points[i]->y;
                auto yip1 = points[next]->y;
                sum_x += (xi + xip1)*(xi * yip1 - xip1*yi);
                sum_y += (yi + yip1)*(xi * yip1 - xip1*yi);
                sum_area += xi * yip1 - xip1*yi;
            }
            double signed_area = 1.0/2.0*sum_area;
            double centroid_x = 1.0 / 6.0 / signed_area * sum_x;
            double centroid_y = 1.0 / 6.0 / signed_area * sum_y;
            return {centroid_x, centroid_y};
        }
    }
}