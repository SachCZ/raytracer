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
             * Get the faces of the mesh excluding the given face.
             * This is useful for intersection finding if
             * @param face
             * @return
             */
            const std::vector<Face *> &getFaces() const {
                return this->faces;
            }

            //TODO delete this
            int getId() const {
                return this->id;
            }

        private:
            explicit Element(int id, std::vector<Face *> faces) :
                    id(id),
                    faces(std::move(faces)) {}

            int id;
            std::vector<Face *> faces;

            friend class Mesh;
        };
    }
}


#endif //RAYTRACER_ELEMENT_H
