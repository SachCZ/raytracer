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
#include "raytracer/utility/JsonFormatter.h"
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

            std::vector<Intersection> getClosestIntersection(const std::vector<Triangle> &triangles) const;

            template <typename Function>

            std::vector<Intersection> findNext(const Intersection& previous, const Mesh &mesh, Function getDirection){
                this->lastDirection = getDirection(previous);
                auto adjacent = mesh.getAdjacent(previous.triangle);
                adjacent.emplace_back(previous.triangle);
                return getClosestIntersection(adjacent);
            }

            template <typename Function>
            void traceThrough(const Mesh &mesh, Function getDirection) {
                auto previous = getClosestIntersection(mesh.getBoundary());

                while(!previous.empty()) {
                    intersections.emplace_back(previous[0]);
                    previous = this->findNext(previous[0], mesh, getDirection);
                }

                if (this->intersections.empty()){
                    throw std::logic_error("No intersections found! Did you miss the boundary?");
                }
            }

            std::vector<Intersection> getTriangleIntersections(const Triangle &triangle) const;
            const std::vector<Intersection>& getIntersections() const;

            void saveToTxt(const std::string &filename) const;

            void saveToJson(const std::string &filename) const;


        private:
            std::vector<Intersection> intersections;

            std::vector<Intersection> getClosestPoint(const Point &point,
                                                      const std::vector<Intersection> &_intersections) const;

            Vector getNormal(const Vector &vector) const;

            double getParamK(const Edge &edge) const;

            double getParamT(const Edge &edge) const;

            bool isIntersecting(const Edge &edge) const;

            Point getIntersectionPoint(const Edge &edge) const;;

        };
    }
}


#endif //RAYTRACER_RAY_H
