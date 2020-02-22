#ifndef RAYTRACER_FACE_H
#define RAYTRACER_FACE_H

#include <vector>
#include <stdexcept>


#include "Vector.h"
#include "Point.h"
#include "Constants.h"

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

            /**
             * Construct the face using an id and std::vector of points that the face consist of.
             * @param id
             * @param points
             */
            explicit Face(int id, std::vector<Point*> points);

            /**
             * Return the point on boundary if given point is reasonably close to it.
             * @param point to be compared with all face points
             * @return on of the face points if it is close to given point. Else it returns nullptr.
             */
            const Point* isBoundary(const Point& point) const;

            /**
             * Get the if of the face.
             * @return id
             */
            int getId() const {
                return this->id;
            }
        private:
            int id;
            std::vector<Point*> points;
        };
    }
}

#endif //RAYTRACER_FACE_H
