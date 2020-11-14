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
         * Load an mfem mesh from file (vtk or mfem native)
         * @param filename
         * @param generateEdges - see mfem docs
         * @param refine - see mfem docs
         */
        explicit MfemMesh(const std::string &filename, bool generateEdges = true, bool refine = false);

        /**
         * Construct a new mfem::Mesh given two sides and element type. Owns the mfem mesh.
         * @param sideA
         * @param sideB
         * @param elementType (fem::Element::Type::TRIANGLE, fem::Element::Type::QUADRILATERAL)
         */
        MfemMesh(
                SegmentedLine sideA,
                SegmentedLine sideB,
                mfem::Element::Type elementType = mfem::Element::Type::QUADRILATERAL
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
        mfem::Mesh *getMfemMesh() const;

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
        std::map<const Point *, std::vector<Element *>> pointsAdjacentElements;

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

        std::vector<Element *> precalcPointAdjacentElements(const Point *point) const;

        std::map<const Point *, std::vector<Element *>> genPointsAdjacentElements() const;

        void init();
    };

    /**
     * Dump the mfem mesh to a stream
     * @param os
     * @param dt
     * @return
     */
    std::ostream &operator<<(std::ostream &os, const MfemMesh &mesh);

    template<typename Component>
    class IndexedGrid {

    public:
        IndexedGrid(int xSize, int ySize) : xSize(xSize), ySize(ySize) {}

        std::vector<Component> getAdjComps(const Component &comp) const {
            std::vector<Component> comps(4);
            if (!isOnRBord(comp)) {
                comps.emplace_back(Component{comp.index + 1});
            }
            if (!isOnTBord(comp)) {
                comps.emplace_back(Component{comp.index + xSize});
            }
            if (!isOnLBord(comp)) {
                comps.emplace_back(Component{comp.index - 1});
            }
            if (!isOnBBord(comp)) {
                comps.emplace_back(Component{comp.index - xSize});
            }
            return comps;
        }

        bool isOnRBord(const Component &comp) const {
            return (comp.index + 1) % xSize;
        }

        bool isOnTBord(const Component &comp) const {
            return comp.index >= (xSize * ySize) - xSize;
        }

        bool isOnLBord(const Component &comp) const {
            return comp.index % xSize == 0;
        }

        bool isOnBBord(const Component &comp) const {
            return comp.index < xSize;
        }

    private:
        int xSize;
        int ySize;
    };

    struct DevEle {
        int index;
    };

    struct Node {
        int index;
    };

    class IndexedMesh {
    public:
        IndexedMesh(int xSegmentsCount, int ySegmentsCount) :
                eleGrid(xSegmentsCount, ySegmentsCount), nodeGrid(xSegmentsCount + 1, ySegmentsCount + 1) {}

        std::vector<DevEle> getAdjElements(const DevEle& element) const {
            return eleGrid.getAdjComps(element);
        }

        std::vector<DevEle> getAdjElements(const Node& node) const {

        }

        std::vector<Node> getAdjNodes(const Node& node) const {
            return nodeGrid.getAdjComps(node);
        }


    private:
        const IndexedGrid<DevEle> eleGrid;
        const IndexedGrid<Node> nodeGrid;
    };
}


#endif //RAYTRACER_MESH_H
