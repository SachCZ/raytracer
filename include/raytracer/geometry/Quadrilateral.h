#ifndef RAYTRACER_QUADRILATERAL_H
#define RAYTRACER_QUADRILATERAL_H

#include <vector>
#include <stdexcept>

#include "Point.h"
#include "Triangle.h"
#include "Edge.h"
#include "Vector.h"

namespace raytracer {
    namespace geometry {
        /**
         * Class representing a quadrilateral
         */
        class Quadrilateral {
        public:
            /**
             * Construct the quadrilateral using a list of points. It size must be 4 else a
             * std::logic_error is thrown. The order of points is counterclockwise from
             * bottom left corner point.
             * @param points
             */
            explicit Quadrilateral(std::vector<Point> points);

            /**
             * Default constructor.
             * By default all the points are initialized to (0, 0).
             */
            Quadrilateral() : points{4} {}

            std::vector<Triangle> getTriangulation() {
                size_t diagonalPointIndex = 0;
                auto _points = this->points;
                for (size_t i = 0; i < _points.size(); ++i){
                    const auto& A = i == 0 ? _points.back() : _points[i-1];
                    const auto& B = _points[i];
                    const auto& C = i == (_points.size() - 1) ? _points.front() : _points[i+1];

                    if (!angleIsConvex(A, B, C)){
                        diagonalPointIndex = i;
                    }
                }
                auto& j = diagonalPointIndex;
                Triangle firstTriangle({
                    _points[j],
                    _points[(j+1) % _points.size()],
                    _points[(j+2) % points.size()]
                });
                Triangle secondTriangle({
                    _points[(j+2) % points.size()],
                    _points[(j+3) % points.size()],
                    _points[(j+4) % points.size()]
                });
                return {firstTriangle, secondTriangle};
            }

        private:
            std::vector<Point> points;

            /**
             * Parameter order matters, points must be clockwise adjacent
             * @param first
             * @param second
             * @return
             */
            bool angleIsConvex(const Point &A, const Point &B, const Point &C) {
                Vector firstNormal = getInvardNormal(B - A);
                Vector second = C - B;
                return firstNormal * second > 0;
            }

            Vector getInvardNormal(const Vector &vector) {
                return {-vector.y, vector.x};
            }
        };
    }
}


#endif //RAYTRACER_QUADRILATERAL_H
