#ifndef RAYTRACER_ELEMENT_H
#define RAYTRACER_ELEMENT_H


#include "Face.h"

namespace raytracer {
    /**
     * \addtogroup api
     * @{
     */

    /**
     * Class representing a single polygonal element in mesh. Could be 2D/3D given by set of faces (edges, surfaces).
     * @warning
     * Do not construct this manually unless you know what you are doing.
     */
    class Element {
    public:
        /**
         * Get the faces of the Element.
         * Edges in 2D, surfaces in 3D.
         * @return the faces
         */
        const std::vector<Face *> &getFaces() const;

        /**
         * Get the points of the Element.
         * @return the points
         */
        const std::vector<Point *> &getPoints() const {
            return this->points;
        };

        /**
         * Constructor taking an id and std::vector of faces of the element.
         * @warning
         * It is users responsibility for the id to be unique. It is thus advised to not construct instance of Element
         * manually.
         *
         * Clockwise order of faces is preferred in 2D.
         * @param id unique identification
         * @param faces of the element
         */
        explicit Element(int id, std::vector<Face *> faces);

        /**
         * Retrieve the id of element.
         * @return id
         */
        int getId() const;

    private:
        int id;
        std::vector<Face *> faces;
        std::vector<Point *> points;
    };

    Point getElementCentroid(const Element &element);

    /**
     * @}
     */
}


#endif //RAYTRACER_ELEMENT_H
