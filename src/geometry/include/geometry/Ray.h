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
#include <utility/JsonFormatter.h>
#include <fstream>
#include <map>

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

            Intersection findNext(const Intersection& previous, const Mesh &mesh, Function getDirection){
                this->lastDirection = getDirection(previous);
                auto adjacent = mesh.getAdjacent(previous.triangle);
                return getClosestIntersection(adjacent);
            }

            template <typename Function>
            void traceThrough(const Mesh &mesh, Function getDirection) {
                try {
                    auto previous = getClosestIntersection(mesh.getBoundary());
                    this->intersections.emplace_back(previous);

                    do {
                        previous = findNext(previous, mesh, getDirection);
                        intersections.emplace_back(previous);
                    } while (intersections.size() < 2 || !mesh.isOnBoundary(previous.triangle));
                    //Try at least three points before concluding that the ray hit the boundary again

                    previous = getClosestIntersection({previous.triangle});
                    this->intersections.emplace_back(previous);

                } catch (const std::logic_error&){
                    throw std::logic_error("The ray missed the target!");
                }
            }

            std::vector<Intersection> getTriangleIntersections(const Triangle &triangle) const;
            const std::vector<Intersection>& getIntersections() const;

            void saveToTxt(const std::string &filename) const;

            void saveToJson(const std::string &filename) const;


        private:
            std::vector<Intersection> intersections;

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
