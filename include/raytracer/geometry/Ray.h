#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <string>
#include "raytracer/utility/JsonFormatter.h"

#include "Point.h"
#include "Edge.h"
#include "Quadrilateral.h"
#include "Mesh.h"

namespace raytracer {
    namespace geometry {
        /**
         * Simple data structure representing a single intersection
         */
        struct Intersection {
            Point point{};
            Edge edge{};
            Quadrilateral quadrilateral{};
        };

        struct IntersVec {
            Point point{};
            Edge edge{};
            Vector direction{};
        };

        struct HalfLine {
            Point point{};
            Vector direction{};
        };

        /**
         * Simple data structure representing the current state of the ray
         * (info regarding place where it last intersected)
         */
        struct RayState {
            Vector currentDirection{};
            Intersection lastIntersection;
        };

        /**
         * Class representing a ray traveling through a mesh. It is first given by point and direction and
         * after raytracing by list of intersectios
         */
        class Ray {
        public:
            /**
             * Construct the ray using a start point and the ray direction.
             * @param startPoint
             * @param direction
             */
            Ray(Point startPoint, Vector direction);

            /**
             * A point from which the ray originates
             */
            Point startPoint;

            /**
             * The direction of the ray end.
             */
            Vector lastDirection; //TODO make this private


            const Quadrilateral* chooseNextElement(const Mesh& mesh, const IntersVec& prevIntersVec){
                auto adjacentElements = mesh.getAdjacent(prevIntersVec.edge);
                for (const auto& adjacentElement : adjacentElements){
                    if (prevIntersVec.direction * adjacentElement.connection >= 0){
                        return &adjacentElement.element;
                    }
                }
                return nullptr;
            }

            using IntersVecPtr = std::unique_ptr<IntersVec>;

            template<typename IntersFunc>
            IntersVecPtr nextIntersVec(const Mesh& mesh, const IntersVec& prevIntersVec, IntersFunc findInters){
                auto nextElement = chooseNextElement(mesh, prevIntersVec);
                if (!nextElement) return nullptr;
                return findInters(prevIntersVec, nextElement);
            }

            template<typename IntersFunc, typename StopCondition>
            void _traceThrough(const Mesh &mesh, IntersFunc findInters, StopCondition stopCondition){
                IntersVecPtr intersVec;

                while (intersVec && !stopCondition(*intersVec)){
                    this->intersVecs.emplace_back(*intersVec);
                    intersVec.reset(nextIntersVec(mesh, *intersVec, findInters));
                }
            }

            /**
             * Get list of all the intersections
             * This wont have any value unless traceThrough was called.
             * @return list of intersections
             */
            const std::vector<IntersVec> & getIntersections() const;

            /**
             * Saves the Ray to txt file.
             * Just the intersection points are saved simply by writing
             * each point coordinates on a new line separated by space
             * eg.: 1 2 etc.
             * @param filename without suffix (.txt will be appended)
             */
            void saveToTxt(const std::string &filename) const;

            /**
             * Save to file using JSON.
             * One JSON object is stored. It is just points: [...],
             * where [..] is a sequence of points eg. [[2, 3], [1, -1]]
             * @param filename withnout suffix (.json will be appended)
             */
            utility::json::Value getJsonValue() const;


        private:
            std::vector<IntersVec> intersVecs;

            Vector getNormal(const Vector &vector) const;

            double getParamK(const Edge &edge, const HalfLine& halfLine) const;

            double getParamT(const Edge &edge, const HalfLine& halfLine) const;

            bool isIntersecting(const Edge &edge, const HalfLine& halfLine) const;

            Point getIntersectionPoint(const Edge &edge, const HalfLine& halfLine) const;
        };
    }
}


#endif //RAYTRACER_RAY_H
