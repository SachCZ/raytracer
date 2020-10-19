#ifndef RAYTRACER_MESH_H
#define RAYTRACER_MESH_H

#include <vector>
#include <memory>
#include "geometry_primitives.h"
#include "mfem.hpp"


namespace raytracer {
    /**
     * Line divided into same size segments
     */
    struct SegmentedLine {
        /**How long is the line.*/
        double length;

        /**
         * Number of line segments
         */
        size_t segmentCount;
    };

    /**
     * Mesh interface
     */
    class Mesh {
    public:

        /**
         * Override this.
         * @param face
         * @param direction
         * @return
         */
        virtual Element *getFaceAdjacentElement(const Face *face, const Vector &direction) const = 0;

        /**
         * Override this.
         * @param element
         * @return
         */
        virtual std::vector<Element *> getElementAdjacentElements(const Element &element) const = 0;

        /**
         * Override this.
         * @return
         */
        virtual std::vector<Face *> getBoundary() const = 0;

        /**
         * Override this.
         * @return
         */
        virtual std::vector<Point *> getInnerPoints() const = 0;

        /**
         * Override this.
         * @return
         */
        virtual std::vector<Point *> getPoints() const = 0;

        /**
         * Override this.
         * @return
         */
        virtual std::vector<Element *> getElements() const = 0;

        /**
         * Override this.
         * @param point
         * @return
         */
        virtual std::vector<Element *> getPointAdjacentElements(const Point *point) const = 0;
    };

    /**
     * Class representing a mesh (2D for now).
     * It encapsulates the class mfem::Mesh and provides some convenience methods.
     */
    class MfemMesh : public Mesh {
    public:
        /**
         * Encapsulate an mfem mesh while not owning it
         */
        explicit MfemMesh(mfem::Mesh *mesh);

        /**
         * Construct a new mfem::Mesh given two sides and element type. Owns the mfem mesh.
         * @param sideA
         * @param sideB
         * @param elementType (fem::Element::Type::TRIANGLE, fem::Element::Type::QUADRILATERAL)
         */
        MfemMesh(
                SegmentedLine sideA,
                SegmentedLine sideB,
                mfem::Element::Type elementType = mfem::Element::Type::TRIANGLE
        );

        /** Given a Face return the adjacent Element to this face in given direction.
         *  It is expected that there are two or less elements adjacent to the face. If there is no
         *  element adjacent in given direction, nullptr is returned.
         *
         * @param face whose adjacent elements are to be found.
         * @param direction in which to search for elements.
         * @return The Element pointer if found or nullptr if not.
         */
        Element *getFaceAdjacentElement(const Face *face, const Vector &direction) const override;

        /**
         * Given an Element return elements adjacent to this element.
         * @param element
         * @return list of element pointers
         */
        std::vector<Element *> getElementAdjacentElements(const Element &element) const override;

        /**
         * Return faces that are on the mesh boundary.
         * @return vector of faces.
         */
        std::vector<Face *> getBoundary() const override;

        /**
         * Return points that are not on the boundary of the mesh
         * @return sequence of points
         */
        std::vector<Point *> getInnerPoints() const override;

        /**
         * Return all mesh points
         * @return sequence of points
         */
        std::vector<Point *> getPoints() const override;

        /**
         * Return all mesh elements
         * @return
         */
        std::vector<Element *> getElements() const override;

        /**
         * Return elements that do share a point
         * @param point
         * @return sequence of elements
         */
        std::vector<Element *> getPointAdjacentElements(const Point *point) const override;

        /**
         * Get pointer to the underlying mfem::Mesh
         * @return
         */
        mfem::Mesh *getMfemMesh();

    private:
        std::unique_ptr<mfem::Mesh> mfemMesh;
        mfem::Mesh *mesh;
        std::vector<Face *> boundaryFaces;
        std::vector<std::unique_ptr<Element>> elements;
        std::vector<std::unique_ptr<Face>> faces;
        std::vector<std::unique_ptr<Point>> points;
        std::vector<Point *> innerPoints;
        mutable mfem::Table elementToElementTable;
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

        std::vector<Point *> genInnerPoints();

        void init();
    };
}


#endif //RAYTRACER_MESH_H
