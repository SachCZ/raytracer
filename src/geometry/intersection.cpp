#include <limits>
#include <algorithm>
#include "intersection.h"

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

    bool impl::isIntersecting(double k, double t, bool includePoint) {
        if (!includePoint) {
            return k >= 0 && k <= 1 && t > 0;
        } else {
            return k >= 0 && k <= 1 && t >= 0;
        }
    }

    PointOnFacePtr
    impl::getClosest(std::vector<PointOnFacePtr> &intersections, const Point &point) {
        static int currentId = 0;
        PointOnFacePtr result = nullptr;
        auto distance = std::numeric_limits<double>::infinity();
        for (auto &pointOnFace : intersections) {
            if (pointOnFace) {
                auto norm = (pointOnFace->point - point).getNorm();
                if (norm <= distance) {
                    result = std::move(pointOnFace);
                    distance = norm;
                }
            }
        }
        if (result){
            result->id = currentId++;
        }
        return result;
    }

    PointOnFacePtr
    findIntersectionPoint(const HalfLine &halfLine, const Face *face, bool includePoint) {
        const auto &points = face->getPoints();
        if (points.size() == 2) {
            double k = impl::getParamK(halfLine, points);
            double t = impl::getParamT(halfLine, points);

            if (impl::isIntersecting(k, t, includePoint)) {
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

    PointOnFacePtr
    findClosestIntersectionPoint(const HalfLine &halfLine, const std::vector<Face *> &faces) {

        std::vector<PointOnFacePtr> intersections;
        std::transform(
                faces.begin(),
                faces.end(),
                std::back_inserter(intersections),
                [&halfLine](const auto &face) { return findIntersectionPoint(halfLine, face); }
        );
        auto result = impl::getClosest(intersections, halfLine.origin);

        if (!result) {
            intersections.clear();
            std::transform(
                    faces.begin(),
                    faces.end(),
                    std::back_inserter(intersections),
                    [&halfLine](const auto &face) { return findIntersectionPoint(halfLine, face, true); }
            );
            result = impl::getClosest(intersections, halfLine.origin);
        }
        return result;
    }
}