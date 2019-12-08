#ifndef RAYTRACER_FACE_H
#define RAYTRACER_FACE_H

#include <vector>
#include <stdexcept>


#include "Vector.h"
#include "Point.h"

namespace raytracer {
    namespace geometry {
        class Mesh;
        class Element;

        /** Class representing a face in mesh (edge in 2D, surface face in 3D).
         *  Instance of this object should not be initialized by user.
         */
        class Face {
        public:
            /** Calculate a normal to the face (edge in 2D).
             *  By convention in 2D, the normal is outward for points
             *  in clockwise order forming a polygon.
             *  @return the normal vector.
             */
            Vector getNormal() const;

            /** Get the points forming the face.
             *
             * @return the points.
             */
            const std::vector<Point*>& getPoints() const;

        private:
            explicit Face(int id, std::vector<Point*> points);

            int id;
            std::vector<Point*> points;

            friend class Mesh;
            friend class Element;
        };
    }
}

#endif //RAYTRACER_FACE_H
