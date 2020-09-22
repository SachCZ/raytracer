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
     * \addtogroup api
     * @{
     */

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


    class Mesh {
    public:

        /**
         * Override this.
         *
         * @param face
         * @param direction
         * @return
         */
        virtual Element *getFaceAdjacentElement(const Face *face, const Vector &direction) const = 0;

        /**
         * Override this.
         *
         * @param element
         * @return
         */
        virtual std::vector<Element *> getElementAdjacentElements(const Element &element) const = 0;

        /**
         * Override this.
         *
         * @return
         */
        virtual std::vector<Face *> getBoundary() const = 0;

        /**
         * Override this.
         *
         * @return
         */
        virtual std::vector<Point *> getInnerPoints() const = 0;

        /**
         * Override this.
         *
         * @return
         */
        virtual std::vector<Point *> getPoints() const = 0;

        /**
         * Override this.
         *
         * @param point
         * @return
         */
        virtual std::vector<Element *> getPointAdjacentElements(const Point *point) const = 0;
    };

    /**
     * Construct a mfem::Mesh given two discrete lines (the sides of the rectangle).
     * The mesh will be a rectangular equidistant grid beginning at (0, 0).
     *
     * @param sideA of the mesh
     * @param sideB of the mesh
     * @return the unique pointer to the mfem::Mesh
     */
    std::unique_ptr<mfem::Mesh>
    constructMfemMesh(
            DiscreteLine sideA,
            DiscreteLine sideB,
            mfem::Element::Type elementType = mfem::Element::Type::TRIANGLE
    );

    /**
     * Class representing a mesh (2D for now).
     * It encapsulates the class mfem::Mesh and provides some convenience methods.
     */
    class MfemMesh : public Mesh {
    public:
        /**
         * Create a rectangular mesh given
         */
        explicit MfemMesh(mfem::Mesh *mesh);

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
         * Return a sequence of faces that are on the mesh boundary.
         * @return sequence of faces.
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
        std::vector<Point *> getPoints() const override {
            std::vector<Point *> result;
            std::transform(this->points.begin(), this->points.end(), std::back_inserter(result), [](const auto &point) {
                return point.get();
            });
            return result;
        }

        /**
         * Return elements that do share a point
         * @param point
         * @return sequence of elements
         */
        std::vector<Element *> getPointAdjacentElements(const Point *point) const override;

    private:
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
    };

    /**
     * @}
     */
}


#endif //RAYTRACER_MESH_H
