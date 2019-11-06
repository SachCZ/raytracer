#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include "Triangle.h"
#include <vector>

namespace raytracer {
    namespace geometry {

        class Mesh {
        public:
            std::vector<Triangle> getBoundary() const {return {};}
            std::vector<Triangle> getAdjacent(const Triangle&) const {return {};}
            bool isOnBoundary(const Triangle&) const {return false;}
        };
    }
}


#endif //RAYTRACER_MESH_H
