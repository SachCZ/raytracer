#ifndef RAYTRACER_FACE_H
#define RAYTRACER_FACE_H

#include <vector>

#include "Vector.h"
#include "Point.h"

namespace raytracer {

    /**
     * \addtogroup api
     * @{
     */

    class MfemMesh;

    class Element;

    /**
     * Class representing a polygonal face in mesh (edge in 2D, surface polygon face in 3D).
     * @warning
     * Do not construct this manually unless you know what you are doing.
     */
    class Face {
    public:
        /**
         *  Calculate a normal to the face (edge in 2D).
         *  By convention in 2D, the normal is outward for points
         *  in clockwise order forming a polygon.
         *  @return the normal vector.
         */
        Vector getNormal() const;

        /**
         * Get the points forming the face.
         *
         * @return the points.
         */
        const std::vector<Point *> &getPoints() const;

        /**
         * Construct the face using an id and std::vector of points that the face consist of.
         *
         * @warning
         * It is users responsibility for the id to be unique. It is thus advised to not construct instance of Face
         * manually.
         *
         * @param id unique identification
         * @param points the face consists of
         */
        explicit Face(int id, std::vector<Point *> points);

        /**
         * Get the id of the face.
         * @return id
         */
        int getId() const;

    private:
        int id;
        std::vector<Point *> points;
    };

    /**
     * @}
     */
}

#endif //RAYTRACER_FACE_H
