#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include <vector>

#include "Vector.h"
#include "Point.h"
#include "Face.h"
#include "GeometryFunctions.h"
#include <mfem.hpp>

/**
 * Namespace of the whole library
 */
namespace raytracer {
    /**
     * Namespace encapsulating all the abstractions related to geometry
     */
    namespace geometry {
        /** Structure representing a discretisation of a distance.
         * That is how many equidistant nodes are on the line with given width.
         */
        struct DiscreteLine {
            /**Length of the line*/
            double length;
            /**Number of segments of the line.
             * Eg. DiscreteLine split by four points has three segments.
             */
            size_t segmentCount;
        };

        /**
         * Construct a mfem::Mesh given two discrete lines (the sides of the rectangle).
         * The mesh will be a rectangular equidistant grid.
         *
         * @param sideA of the mesh
         * @param sideB of the mesh
         * @return the unique pointer to the mesh
         */
        std::unique_ptr<mfem::Mesh> constructRectangleMesh(DiscreteLine sideA, DiscreteLine sideB);

        /**
         * Class representing a mesh. For now it is a mesh of quadrilaterals.
         * It encapsulates the class mfem::Mesh and provides some convenience methods.
         */
        class Mesh {
        public:
            /** Create a rectangular mesh given
             */
            explicit Mesh(mfem::Mesh *mesh);

            /** Given a face return the adjacent element to this face in given direction.
             *  It is expected that there are two or less elements adjacent to the face. If there is no
             *  element adjacent in given direction, nullptr is returned.
             *  If the orientation point is exactly in the face corner point a special check is made to find
             *  a possibly diagonal element.
             * @param face whose adjacent elements are to be found.
             * @param orientation in which to search for elements.
             * @return The element pointer if found or nullptr if not.
             */
            Element *getAdjacentElement(const Face *face, const HalfLine &orientation) const;

            /**
             * Return a sequence of faces that are on the mesh boundary.
             * @return sequence of faces.
             */
            std::vector<Face *> getBoundary() const;

        private:
            mfem::Mesh *mesh;
            std::vector<Face *> boundaryFaces;
            std::vector<std::unique_ptr<Element>> elements;
            std::vector<std::unique_ptr<Face>> faces;
            std::vector<std::unique_ptr<Point>> points;

            Element * getFaceAdjacentElement(const Face *face, const Vector &direction) const;


            std::unique_ptr<Point> createPointFromId(int id) const;

            std::unique_ptr<Face> createFaceFromId(int id) const;

            std::unique_ptr<Element> createElementFromId(int id) const;


            std::vector<Point *> getPointsFromIds(const mfem::Array<int> &ids) const;

            std::vector<Face *> getFacesFromIds(const mfem::Array<int> &ids) const;

            Element *getElementFromId(int id) const;

            std::vector<std::unique_ptr<Point>> genPoints();

            std::vector<std::unique_ptr<Face>> genFaces();

            std::vector<std::unique_ptr<Element>> genElements();

            std::vector<Face *> genBoundaryFaces();
        };
    }
}

#endif //RAYTRACER_MESH_H
