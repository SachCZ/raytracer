#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

/**
 * Namespace of the whole library
 */
#include <vector>

#include "Vector.h"
#include "Point.h"
#include <mfem.hpp>

namespace raytracer {
    /**
     * Namespace encapsulating all the abstractions related to geometry
     */
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
            Vector getNormal() const {
                if (this->points.size() == 2){
                    auto direction = points[1] - points[0];
                    return direction.getNormal();
                } else {
                    throw std::logic_error("Can get normal to face!");
                }
            }

            /** Get the points forming the face.
             *
             * @return the points.
             */
            const std::vector<Point>& getPoints() const {
                return this->points;
            }
        private:
            explicit Face(int id, std::vector<Point> points) :
                    id(id),
                    points(std::move(points)) {}

            int id;
            std::vector<Point> points;

            friend class Mesh;
            friend class Element;
        };

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
            size_t nodeCount;
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
            explicit Mesh(DiscreteLine sideA, DiscreteLine sideB) {
                this->mesh = std::make_unique<mfem::Mesh>(
                        sideA.nodeCount,
                        sideB.nodeCount,
                        mfem::Element::Type::QUADRILATERAL,
                        true,
                        sideA.width,
                        sideB.width,
                        true);
                this->boundaryFacesIds = Mesh::genBoundaryFacesIds(this->mesh.get());
            }

            /** Given a face return the adjacent element to this face in given direction.
             *  It is expected that there are two or less elements adjacent to the face. If there is no
             *  element adjacent in given direction, nullptr is returned.
             * @param face whose adjacent elements are to be found.
             * @param direction in which to search for elements.
             * @return The element pointer if found or nullptr if not.
             */
            std::unique_ptr<Element> getAdjacentElement(const Face &face, const Vector &direction) const {
                int elementA, elementB;
                this->mesh->GetFaceElements(face.id, &elementA, &elementB);

                auto normal = face.getNormal();

                if (normal * direction < 0) {
                    return this->getElementFromId(elementA);
                } else {
                    return this->getElementFromId(elementB);
                }
            }

            std::vector<Face> getBoundary() const {
                return this->getFacesFromIds(this->boundaryFacesIds);
            }

        private:
            std::unique_ptr<mfem::Mesh> mesh;
            std::vector<int> boundaryFacesIds;

            std::unique_ptr<Element> getElementFromId(int id) const {
                if (id == -1) return nullptr;

                mfem::Array<int> facesIds;
                mfem::Array<int> _;

                if (this->mesh->Dimension() == 2) {
                    this->mesh->GetElementEdges(id, facesIds, _);
                } else if (this->mesh->Dimension() == 3){
                    this->mesh->GetElementFaces(id, facesIds, _);
                }

                //Not using make_unique, because Element constructor is private.
                std::unique_ptr<Element> element(new Element(id, this->getFacesFromIds(facesIds)));
                return element;
            }

            Point getPointFromId(int id) const { //TODO refactor this, dimension specific
                std::vector<double> coords(this->mesh->Dimension());
                mesh->GetNode(id, &coords[0]);
                return {coords[0], coords[1]};
            }

            std::vector<Point> getPointsFromIds(const mfem::Array<int> &ids) const {
                std::vector<Point> result;
                for (auto id : ids) {
                    result.emplace_back(this->getPointFromId(id));
                }
                return result;
            }

            Face getFaceFromId(int id) const {
                mfem::Array<int> faceVerticesIds;
                this->mesh->GetFaceVertices(id, faceVerticesIds);

                return Face(id, this->getPointsFromIds(faceVerticesIds));
            }

            template <typename Sequence>
            std::vector<Face> getFacesFromIds(const Sequence &ids) const {
                std::vector<Face> result;
                size_t size = std::distance(ids.begin(), ids.end());
                result.reserve(size);
                for (auto id : ids) {
                    result.emplace_back(this->getFaceFromId(id));
                }
                return result;
            }

            static std::vector<int> genBoundaryFacesIds(const mfem::Mesh* _mesh) {
                std::vector<int> result;
                for (int i = 0; i < _mesh->GetNumFaces(); ++i){
                    int id1, id2;
                    _mesh->GetFaceElements(i, &id1, &id2);
                    if (id1 == -1 || id2 == -1){
                        result.emplace_back(i);
                    }
                }
                return result;
            }
        };
    }
}


#endif //RAYTRACER_MESH_H
