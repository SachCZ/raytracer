#include <limits>
#include "Intersection.h"

namespace raytracer {

    double impl::getParamK(const HalfLine &halfLine,
                           const std::vector<Point *> &points) {
        auto normal = halfLine.direction.getNormal();
        const auto &P = halfLine.origin;
        const auto &A = *points[0];
        const auto &B = *points[1];
        return (normal * (P - A)) / (normal * (B - A));
    }

    double impl::getParamT(const HalfLine &halfLine,
                           const std::vector<Point *> &points) {
        const auto &A = *points[0];
        const auto &B = *points[1];
        auto normal = (B - A).getNormal();
        const auto &d = halfLine.direction;
        const auto &P = halfLine.origin;

        return (normal * (A - P)) / (normal * d);
    }

    bool impl::isIntersecting(double k, double t) {
        return k >= 0 && k <= 1 && t >= 0;
    }

    std::unique_ptr<PointOnFace>
    findIntersection(const HalfLine &halfLine,
                     const Face *face) {
        const auto &points = face->getPoints();
        if (points.size() == 2) {
            double k = impl::getParamK(halfLine, points);
            double t = impl::getParamT(halfLine, points);

            if (impl::isIntersecting(k, t)) {
                const auto &A = *points[0];
                const auto &B = *points[1];
                auto point = (A + k * (B - A));
                auto pointOnFace = std::make_unique<PointOnFace>();
                pointOnFace->point = point;
                pointOnFace->face = face;

                return pointOnFace;
            } else {
                return nullptr;
            }
        } else {
            throw std::logic_error("Can get face intersection!");
        }
    }

    std::unique_ptr<PointOnFace>
    findClosestIntersection(const HalfLine &halfLine,
                            const std::vector<Face *> &faces,
                            const Face *omitFace) {

        std::unique_ptr<PointOnFace> result = nullptr;
        auto distance = std::numeric_limits<double>::infinity();


        for (const auto &face : faces) {
            if (face == omitFace) continue;
            auto pointOnFace = findIntersection(halfLine, face);
            if (pointOnFace) {
                auto norm = (pointOnFace->point - halfLine.origin).getNorm();
                if (norm < std::numeric_limits<double>::epsilon()) {
                    norm = std::numeric_limits<double>::infinity();//If it is too close, prefer others
                    //TODO I dont like this at all
                    //TODO Easily solvable with triangles
                    //TODO not so easy with quads
                }
                if (norm <= distance) {
                    result = std::move(pointOnFace);
                    distance = norm;
                }
            }
        }
        return result;
    }
}