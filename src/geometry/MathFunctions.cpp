#include "GeometryFunctions.h"

double raytracer::geometry::impl::getParamK(const HalfLine &halfLine,
                                            const std::vector<raytracer::geometry::Point*> &points) {
    auto normal = halfLine.direction.getNormal();
    const auto &P = halfLine.point;
    const auto &A = *points[0];
    const auto &B = *points[1];
    return (normal * (P - A)) / (normal * (B - A));
}

double raytracer::geometry::impl::getParamT(const HalfLine &halfLine,
                                            const std::vector<raytracer::geometry::Point*> &points) {
    const auto &A = *points[0];
    const auto &B = *points[1];
    auto normal = (B - A).getNormal();
    const auto &d = halfLine.direction;
    const auto &P = halfLine.point;

    return (normal * (A - P)) / (normal * d);
}

bool raytracer::geometry::impl::isIntersecting(double k, double t) {
    return k > -constants::epsilon && k < 1 + constants::epsilon && t > constants::epsilon;
}

std::unique_ptr<raytracer::geometry::Intersection>
raytracer::geometry::findIntersection(const HalfLine &halfLine,
                                      const Face *face) {
    const auto& points = face->getPoints();
    if (points.size() == 2){
        double k = impl::getParamK(halfLine, points);
        double t = impl::getParamT(halfLine, points);

        if (impl::isIntersecting(k, t)) {
            auto point = (halfLine.point + t*halfLine.direction);
            auto direction = halfLine.direction;
            HalfLine newHalfLine{point, direction};
            auto intersection = std::make_unique<Intersection>();
            intersection->orientation = newHalfLine;
            intersection->face = face;

            return intersection;
        } else {
            return nullptr;
        }
    } else {
        throw std::logic_error("Can get face intersection!");
    }
}

std::unique_ptr<raytracer::geometry::Intersection>
raytracer::geometry::findClosestIntersection(const HalfLine &halfLine,
                                             const std::vector<Face *> &faces) {

    std::unique_ptr<Intersection> result = nullptr;
    auto distance = std::numeric_limits<double>::infinity();


    for (const auto& face : faces){
        auto intersection = findIntersection(halfLine, face);
        if (intersection) {
            auto norm = (intersection->orientation.point - halfLine.point).getNorm();
            if (norm < distance){
                result = std::move(intersection);
                distance = norm;
            }
        }
    }
    return result;
}
