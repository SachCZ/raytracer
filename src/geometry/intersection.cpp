#include <limits>
#include <algorithm>
#include <utility.h>
#include "intersection.h"

namespace raytracer {


    double getParamK(const Ray &ray,
                           const std::vector<Point *> &points) {
        auto normal = ray.direction.getNormal();
        const auto &P = ray.origin;
        const auto &A = *points[0];
        const auto &B = *points[1];
        return (normal * (P - A)) / (normal * (B - A));
    }

    double getParamT(const Ray &ray,
                           const std::vector<Point *> &points) {
        const auto &A = *points[0];
        const auto &B = *points[1];
        auto normal = (B - A).getNormal();
        const auto &d = ray.direction;
        const auto &P = ray.origin;

        return (normal * (A - P)) / (normal * d);
    }

    bool isIntersecting(double k, double t, bool includePoint) {
        if (!includePoint) {
            return k >= 0 && k <= 1 && t > 0;
        } else {
            return k >= 0 && k <= 1 && t >= 0;
        }
    }

    PointOnFacePtr getClosest(std::vector<PointOnFacePtr> &intersections, const Point &point) {
        static int currentId = 0;
        PointOnFacePtr result = nullptr;
        auto distance2 = std::numeric_limits<double>::infinity();
        for (auto &pointOnFace : intersections) {
            if (pointOnFace) {
                auto norm2 = (pointOnFace->point - point).getNorm2();
                if (norm2 <= distance2) {
                    result = std::move(pointOnFace);
                    distance2 = norm2;
                }
            }
        }
        if (result){
            result->id = currentId++;
        }
        return result;
    }

    PointOnFacePtr findIntersectionPoint(const Ray &ray, const Face *face, bool includePoint = false) {
        const auto &points = face->getPoints();
        if (points.size() == 2) {
            const double k = getParamK(ray, points);
            const double t = getParamT(ray, points);

            if (isIntersecting(k, t, includePoint)) {
                const auto &A = *points[0];
                const auto &B = *points[1];
                auto point = Point(Vector(A) + k * (B - A));
                auto pointOnFace = make_unique<PointOnFace>();
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

    PointOnFacePtr findClosestIntersectionPoint(const Ray &ray, const std::vector<Face *> &faces) {

        std::vector<PointOnFacePtr> intersections;
        intersections.reserve(faces.size());
        std::transform(
                faces.begin(),
                faces.end(),
                std::back_inserter(intersections),
                [&ray](const Face* face) { return findIntersectionPoint(ray, face); }
        );
        auto result = getClosest(intersections, ray.origin);

        if (!result) {
            intersections.clear();
            std::transform(
                    faces.begin(),
                    faces.end(),
                    std::back_inserter(intersections),
                    [&ray](const Face* face) { return findIntersectionPoint(ray, face, true); }
            );
            result = getClosest(intersections, ray.origin);
        }
        return result;
    }
}