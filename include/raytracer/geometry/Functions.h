#ifndef RAYTRACER_FUNCTIONS_H
#define RAYTRACER_FUNCTIONS_H

#include <memory>

#include "Point.h"
#include "Vector.h"
#include "Face.h"
#include "Constants.h"

namespace raytracer {
    namespace geometry {
        struct HalfLine {
            Point point;
            Vector direction;
        };

        struct Intersection {
            Intersection(const HalfLine& halfLine, Face face):
                    halfLine(halfLine), face(std::move(face))
            {}

            HalfLine halfLine;
            Face face;
        };

        namespace impl {
            double getParamK(const HalfLine &halfLine, const std::vector<Point> &points);

            double getParamT(const HalfLine &halfLine, const std::vector<Point> &points);

            bool isIntersecting(double k, double t);
        }

        std::unique_ptr<Intersection> findIntersection(const HalfLine &halfLine, const Face &face);

        std::unique_ptr<Intersection> findClosestIntersection(const HalfLine& halfLine, const std::vector<Face>& faces);
    }
}

#endif //RAYTRACER_FREE_FUNCTIONS_H
