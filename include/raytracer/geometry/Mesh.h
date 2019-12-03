#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include <vector>

#include "Vector.h"
#include "Point.h"
#include "Face.h"
#include <mfem.hpp>

/**
 * Namespace of the whole library
 */
namespace raytracer {
    /**
     * Namespace encapsulating all the abstractions related to geometry
     */
    namespace geometry {
        class Mesh;

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
            const std::vector<Face>& getFaces() const {
                return this->faces;
            }

            //TODO delete this
            int getId() const {
                return this->id;
            }
        private:
            explicit Element(int id, std::vector<Face> faces) :
                    id(id),
                    faces(std::move(faces)) {}

            int id;
            std::vector<Face> faces;

            friend class Mesh;
        };

        /** Structure representing a discretisation of a distance.
         * That is how many equidistant nodes are on the line with given width.
         */
        struct DiscreteLine {
            double width;
            size_t segmentCount;
        };

        /**
         * Class representing a mesh. For now it is a mesh of quadrilaterals.
         * It encapsulates the class mfem::Mesh and provides some convenience methods.
         */
        class Mesh {
        public:
            /** Create a rectangular mesh given two discrete lines (the sides of the rectangle).
             * The mesh will be a rectangular equidistant grid.
             *
             * @param sideA
             * @param sideB
             */
            explicit Mesh(DiscreteLine sideA, DiscreteLine sideB);

            /** Given a face return the adjacent element to this face in given direction.
             *  It is expected that there are two or less elements adjacent to the face. If there is no
             *  element adjacent in given direction, nullptr is returned.
             * @param face whose adjacent elements are to be found.
             * @param direction in which to search for elements.
             * @return The element pointer if found or nullptr if not.
             */
            std::unique_ptr<Element> getAdjacentElement(const Face &face, const Vector &direction) const;

            /**
             * Return a sequence of faces that are on the mesh boundary.
             * @return sequence of faces.
             */
            std::vector<Face> getBoundary() const;

        private:
            std::unique_ptr<mfem::Mesh> mesh;
            std::vector<int> boundaryFacesIds;

            std::unique_ptr<Element> getElementFromId(int id) const;

            Point getPointFromId(int id) const;

            std::vector<Point> getPointsFromIds(const mfem::Array<int> &ids) const;

            Face getFaceFromId(int id) const;

            template <typename Sequence>
            std::vector<Face> getFacesFromIds(const Sequence &ids) const;

            static std::vector<int> genBoundaryFacesIds(const mfem::Mesh* _mesh);
        };
    }
}

//Template definitions
template <typename Sequence>
std::vector<raytracer::geometry::Face> raytracer::geometry::Mesh::getFacesFromIds(const Sequence &ids) const {
    std::vector<Face> result;
    size_t size = std::distance(ids.begin(), ids.end());
    result.reserve(size);
    for (auto id : ids) {
        result.emplace_back(this->getFaceFromId(id));
    }
    return result;
}

#endif //RAYTRACER_MESH_H
