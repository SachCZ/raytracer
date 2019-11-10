#include "Ray.h"

raytracer::geometry::Ray::Ray(raytracer::geometry::Point startPoint, raytracer::geometry::Vector direction)
        : startPoint(startPoint), lastDirection(direction) {
}

raytracer::geometry::Point raytracer::geometry::Ray::getLastPoint() const {
    return intersections.empty() ? startPoint : intersections.back().point;
}

raytracer::geometry::Intersection
raytracer::geometry::Ray::getClosestIntersection(const std::vector<raytracer::geometry::Triangle> &triangles) const {
    std::vector<Intersection> _intersections;
    std::for_each(triangles.begin(), triangles.end(), [&](const Triangle &triangle) {
        auto points = this->getTriangleIntersections(triangle);
        _intersections.insert(_intersections.end(), points.begin(), points.end());
    });
    return getClosestIntersection(this->getLastPoint(), _intersections);
}

std::vector<raytracer::geometry::Intersection>
raytracer::geometry::Ray::getTriangleIntersections(const raytracer::geometry::Triangle &triangle) const {
    const auto &edges = triangle.edges;
    std::vector<Intersection> _intersections;
    std::for_each(
            edges.begin(),
            edges.end(),
            [&](const Edge &edge) {
                if (this->isIntersecting(edge)) {
                    Intersection intersection{
                            this->getIntersectionPoint(edge),
                            edge,
                            triangle
                    };
                    _intersections.emplace_back(intersection);
                }
            });
    return _intersections;
}

raytracer::geometry::Intersection
raytracer::geometry::Ray::getClosestIntersection(const raytracer::geometry::Point &point,
                                                 const std::vector<raytracer::geometry::Intersection> &_intersections) const {
    if (_intersections.empty()) throw std::logic_error("There must be points to choose from!");
    auto closestIntersection = _intersections[0];
    double distance = std::numeric_limits<double>::infinity();
    std::for_each(_intersections.begin(), _intersections.end(), [&](const Intersection &_intersection) {
        auto currentDistance = (_intersection.point - point).getNorm();
        if (currentDistance < distance) {
            distance = currentDistance;
            closestIntersection = _intersection;
        }
    });
    return closestIntersection;
}

raytracer::geometry::Vector raytracer::geometry::Ray::getNormal(const raytracer::geometry::Vector &vector) const {
    return {vector.y, -vector.x};
}

double raytracer::geometry::Ray::getParamK(const raytracer::geometry::Edge &edge) const {
    auto normal = getNormal(this->lastDirection);
    const auto &P = this->getLastPoint();
    const auto &A = edge.startPoint;
    const auto &B = edge.endPoint;

    return (normal * (P - A)) / (normal * (B - A));
}

double raytracer::geometry::Ray::getParamT(const raytracer::geometry::Edge &edge) const {
    const auto &A = edge.startPoint;
    const auto &B = edge.endPoint;
    auto normal = getNormal(B - A);
    const auto &d = this->lastDirection;
    const auto &P = this->getLastPoint();

    return (normal * (A - P)) / (normal * d);
}

bool raytracer::geometry::Ray::isIntersecting(const raytracer::geometry::Edge &edge) const {
    auto k = this->getParamK(edge);
    auto t = this->getParamT(edge);
    return k > -std::numeric_limits<double>::epsilon() && k < 1 + std::numeric_limits<double>::epsilon() && t > std::numeric_limits<double>::epsilon();
}

raytracer::geometry::Point raytracer::geometry::Ray::getIntersectionPoint(const raytracer::geometry::Edge &edge) const {
    const auto &A = edge.startPoint;
    const auto &B = edge.endPoint;

    auto k = this->getParamK(edge);
    return A + (k * (B - A));
}

const std::vector<raytracer::geometry::Intersection> &raytracer::geometry::Ray::getIntersections() const {
    return this->intersections;
}

void raytracer::geometry::Ray::saveToTxt(const std::string &filename) const {
    std::ofstream file(filename + ".txt");
    for (auto& intersection : this->intersections){
        file << intersection.point.x << " " << intersection.point.y << std::endl;
    }
}

void raytracer::geometry::Ray::saveToJson(const std::string &filename) const {
    using formatter = utility::JsonFormatter;

    auto pointsJson = formatter::getSequenceRepresentation(
            this->intersections, [](const Intersection& intersection){
                std::vector<double> coordinates = {intersection.point.x, intersection.point.y};
                return formatter::getSequenceRepresentation(coordinates, [](double x){return x;});
            });

    std::map<std::string, std::string> jsonObject = {{"intersections", pointsJson}};

    std::ofstream file(filename + ".json");
    file << formatter::getObjectRepresentation(jsonObject);
}
