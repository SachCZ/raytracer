#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H

#include <vector>
#include "Point.h"
#include "Edge.h"

namespace raytracer {
    namespace geometry {

        class Mesh;
        class Triangle {
        public:
            explicit Triangle(std::vector<Point> points);
            Triangle(): points{3}{}

            std::vector<Edge> edges;

            const std::vector<Point>& getPoints() const;
        private:
            std::vector<Point> points;
            int id{0};

            void recalculateEdges();

            friend class Mesh;
        };
    }
}


#endif //RAYTRACER_TRIANGLE_H
