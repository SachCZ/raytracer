#include <algorithm>
#include <fstream>

#include "Ray.h"
#include "raytracer/utility/JsonFormatter.h"

raytracer::geometry::Ray::Ray(raytracer::geometry::Point startPoint, raytracer::geometry::Vector direction)
        : startPoint(startPoint), lastDirection(direction) {
}

raytracer::geometry::Vector raytracer::geometry::Ray::getNormal(const raytracer::geometry::Vector &vector) const {
    return {vector.y, -vector.x};
}

double raytracer::geometry::Ray::getParamK(const raytracer::geometry::Edge &edge, const HalfLine& halfLine) const {
    auto normal = getNormal(halfLine.direction);
    const auto &P = halfLine.point;
    const auto &A = *edge.startPoint;
    const auto &B = *edge.endPoint;

    return (normal * (P - A)) / (normal * (B - A));
}

double raytracer::geometry::Ray::getParamT(const raytracer::geometry::Edge &edge, const HalfLine& halfLine) const {
    const auto &A = *edge.startPoint;
    const auto &B = *edge.endPoint;
    auto normal = getNormal(B - A);
    const auto &d = halfLine.direction;
    const auto &P = halfLine.point;

    return (normal * (A - P)) / (normal * d);
}

bool raytracer::geometry::Ray::isIntersecting(const raytracer::geometry::Edge &edge, const HalfLine& halfLine) const {
    auto k = this->getParamK(edge, halfLine);
    auto t = this->getParamT(edge, halfLine);
    return k > -std::numeric_limits<double>::epsilon() && k < 1 + std::numeric_limits<double>::epsilon() && t > std::numeric_limits<double>::epsilon();
}

raytracer::geometry::Point raytracer::geometry::Ray::getIntersectionPoint(
        const raytracer::geometry::Edge &edge,
        const HalfLine& halfLine) const {
    const auto &A = *edge.startPoint;
    const auto &B = *edge.endPoint;

    auto k = this->getParamK(edge, halfLine);
    return A + (k * (B - A));
}

const std::vector<raytracer::geometry::IntersVec> & raytracer::geometry::Ray::getIntersections() const {
    return this->intersVecs;
}

void raytracer::geometry::Ray::saveToTxt(const std::string &filename) const {
    std::ofstream file(filename + ".txt");
    for (auto& intersection : this->intersVecs){
        file << intersection.point.x << " " << intersection.point.y << std::endl;
    }
}

raytracer::utility::json::Value raytracer::geometry::Ray::getJsonValue() const {
    using JsonValue = utility::json::Value;

    JsonValue points;

    for (const auto& intersection : this->intersVecs){
        JsonValue point;
        point.append(JsonValue(intersection.point.x));
        point.append(JsonValue(intersection.point.y));

        points.append(point);
    }
    return points;
}
