#ifndef RAYTRACER_ELEMENT_H
#define RAYTRACER_ELEMENT_H


#include "Face.h"

namespace raytracer {
    namespace geometry {
        /** Class representing a single volume element in mesh.
         *  Could be any element given by set of faces (edges, surfaces).
         * Instance of this object should not be initialized by user.
         */
        class Element {
        public:
            /**
             * Get the faces of the Element.
             * Edges in 2D, surfaces in 3D.
             * @return the face
             */
            const std::vector<Face *> &getFaces() const {
                return this->faces;
            }

            explicit Element(int id, std::vector<Face *> faces) :
                    id(id),
                    faces(std::move(faces)) {}
            int getId() const {
                return this->id;
            }
        private:
            int id;
            std::vector<Face *> faces;
        };
    }
}


#endif //RAYTRACER_ELEMENT_H
