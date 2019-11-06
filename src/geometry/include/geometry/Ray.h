#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include "Point.h"
#include "Vector.h"
#include "Triangle.h"
#include "Mesh.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>

namespace raytracer {
    namespace geometry {
        struct Intersection {
            Point point{};
            Edge edge{};
            Triangle triangle{};
        };

        class Ray {
        public:
            Ray(Point startPoint, Vector direction);

            Point startPoint;
            Vector lastDirection;

            Point getLastPoint() const;

            Intersection getClosestIntersection(const std::vector<Triangle> &triangles) const;

            template <typename Function>
            void traceThrough(const Mesh &mesh, Function getDirection) {
                auto lastIntersection = getClosestIntersection(mesh.getBoundary());
                this->intersections.emplace_back(lastIntersection);

                do {
                    this->lastDirection = getDirection(lastIntersection);
                    auto adjacent = mesh.getAdjacent(lastIntersection.triangle);
                    lastIntersection = getClosestIntersection(adjacent);
                    this->intersections.emplace_back(lastIntersection);

                } while (mesh.isOnBoundary(lastIntersection.triangle));
            }

        private:
            std::vector<Intersection> intersections;

            std::vector<Intersection> getIntersections(const Triangle &triangle) const;

            Intersection getClosestIntersection(const Point &point, const std::vector<Intersection> &_intersections) const;

            Vector getNormal(const Vector &vector) const;

            double getParamK(const Edge &edge) const;

            double getParamT(const Edge &edge) const;

            bool isIntersecting(const Edge &edge) const;

            Point getIntersectionPoint(const Edge &edge) const;;

        };
    }
}


#endif //RAYTRACER_RAY_H
