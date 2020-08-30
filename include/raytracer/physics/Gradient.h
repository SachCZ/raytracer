#ifndef RAYTRACER_GRADIENT_H
#define RAYTRACER_GRADIENT_H

#include "QRDecomposition.h"
#include "Matrix2D.h"
#include "Vector.h"
#include "Intersection.h"
#include "mfem.hpp"
#include "Point.h"
#include "MeshFunction.h"
#include "Mesh.h"
#include "FreeFunctions.h"

namespace raytracer {
    /**
     * \addtogroup gradients
     * @{
     */

    /**
     * Abstract interface to provide a GradientCalculator. To obey this interface implement the
     * get method.
     */
    class Gradient {
    public:
        /**
         * Override this.
         * @return the gradient at given intersection PointOnFace.
         */
        virtual Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const = 0;
    };

    /**
     * GradientCalculator that returns a constant Vector no matter what.
     */
    class ConstantGradient : public Gradient {
    public:
        /**
         * Constructor that takes the Vector that will be returned every time as parameter.
         * @param gradient - the vector to be returned
         */
        explicit ConstantGradient(const Vector &gradient);

        /**
         * Returns always the same Vector given at construction.
         * @return vector gradient.
         */
        Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;

    private:
        const Vector gradient;
    };

    /**
     * GradientCalculator that stores a density grid function defined in H1 space obtained from function given
     * if L2. When get is called the gradient of this function is calculated at given point.
     *
     * @warning
     * Please note that updateDensity() must be called before calling get().
     */
    class H1Gradient : public Gradient {
    public:
        /**
         * Constructor that expects the l2 and h1 spaces. L2 is the space in which density is usually defined
         * and H1 is the space into which the density is transformed to be able to evaluate gradient.
         * @param l2Space
         * @param h1Space
         */
        H1Gradient(
                mfem::FiniteElementSpace &l2Space,
                mfem::FiniteElementSpace &h1Space
        );

        /**
         * Return the value of gradient at the intersection point.
         *
         * @param pointOnFace
         * @param previousElement
         * @param nextElement
         * @return gradient at given point
         */
        Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;


        /**
         * Update the density from which the gradient is calculated. The density should be a function in L2 space.
         * @param density defined over L2
         */
        void updateDensity(mfem::GridFunction &density);

    private:
        mfem::FiniteElementSpace &l2Space;
        mfem::FiniteElementSpace &h1Space;
        mfem::GridFunction _density;

        Vector getGradientAt(const Element &element, const Point &point) const;

        mfem::GridFunction projectL2toH1(mfem::GridFunction &function);
    };

    /** Gradient model computes the gradient as a unit vector normal to the face and pointing int the direction
     * of more dense element
     */
    class NormalGradient : public Gradient {
    public:

        explicit NormalGradient(const MeshFunction &density) : density(density) {}

        /**
         * Return the value of gradient at the intersection point.
         *
         * @param pointOnFace
         * @param previousElement
         * @param nextElement
         * @return gradient at given point
         */
        Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;

    private:
        const MeshFunction &density;
    };

    /**
     * Gradient model computes the gradient by fitting a plane to neighbour cells by means of lest squares.
     * The method is described in Kucharik's article.
     */
    class LeastSquare : public Gradient {
    public:
        /**
         * LeastSquare requires the Mesh to be able to retrieve neighbours of cell
         * and of course the density MeshFunction.
         * @param mesh
         * @param density
         */
        explicit LeastSquare(const Mesh &mesh, const MeshFunction &density);

        /**
         * Return a norm weighted average of gradients computed using least squares method at previous and next element.
         * @param previousElement
         * @param nextElement
         * @return
         */
        Vector get(const PointOnFace &, const Element &previousElement, const Element &nextElement) const override;

    private:
        const Mesh &mesh;
        const MeshFunction &density;

        static Vector normWeightedAverage(const std::vector<Vector> &vectors);

        static std::array<double, 2> getCentroid(const Element &element);

        Vector getValueAt(const Element &element, const std::vector<Element *> &neighbours) const;

        static double getCoeffA(
                const Element &element,
                const std::vector<Element *> &neighbours,
                uint firstAxis, uint secondAxis
        );

        double getCoeffB(const Element &element, const std::vector<Element *> &neighbours, uint axis) const;

        static Vector solve(Matrix2D A, Vector b);
    };

    class Householder : public Gradient {
    public:
        Householder(const Mesh &mesh, const MeshFunction &density, double smoothingFWHM) :
                mesh(mesh), density(density), gaussian(smoothingFWHM) {}

        Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;

        std::map<Point *, Vector> getSmoothedGradientAtPoints();

        void update(bool smoothing = true);

    private:
        const Mesh &mesh;
        const MeshFunction &density;
        std::map<Point *, Vector> gradientAtPoints;
        Gaussian gaussian;

        std::map<Point *, Vector> getGradientAtPoints() const;

        Vector getGradientAtPoint(const Point *point) const;

        static Vector solveOverdetermined(rosetta::Matrix &A, rosetta::Matrix &b);

        static Vector getCentroid(const Element &element);

        static Vector linearInterpolate(
                const Point &a,
                const Point &b,
                const Point &x,
                const Vector &valueA,
                const Vector &valueB);

    };

    /**
     * @}
     */
}


#endif //RAYTRACER_GRADIENT_H
