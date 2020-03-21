#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include <vector>
#include <memory>

#include "Vector.h"
#include "Point.h"
#include "Face.h"
#include "Element.h"
#include "Intersection.h"
#include <mfem.hpp>

/**
 * Namespace of the whole library.
 */
namespace raytracer {

    /**
     * Structure representing a discretization of a distance.
     * That is how many same size segments are on the line with given width.
     */
    struct DiscreteLine {
        /**How long is the line.*/
        double length;
        /**
         * Number of segments on the line.
         * Eg. DiscreteLine split by two points has three segments.
         */
        size_t segmentCount;
    };

    /**
     * Construct a mfem::Mesh given two discrete lines (the sides of the rectangle).
     * The mesh will be a rectangular equidistant grid beginning at (0, 0).
     *
     * @param sideA of the mesh
     * @param sideB of the mesh
     * @return the unique pointer to the mfem::Mesh
     */
    std::unique_ptr<mfem::Mesh> constructRectangleMesh(DiscreteLine sideA, DiscreteLine sideB);
    //TODO this should be constructing raytracer Mesh

    /**
     * Class representing a mesh (2D for now).
     * It encapsulates the class mfem::Mesh and provides some convenience methods.
     */
    class Mesh {
    public:
        /**
         * Create a rectangular mesh given
         */
        explicit Mesh(mfem::Mesh *mesh);

        /** Given a Face return the adjacent Element to this face in given direction.
         *  It is expected that there are two or less elements adjacent to the face. If there is no
         *  element adjacent in given direction, nullptr is returned.
         *
         * @param face whose adjacent elements are to be found.
         * @param direction in which to search for elements.
         * @return The Element pointer if found or nullptr if not.
         */
        Element *getFaceAdjacentElement(const Face *face, const Vector &direction) const;

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
        std::unique_ptr<mfem::Table> vertexToElementTable;

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


#endif //RAYTRACER_MESH_H
