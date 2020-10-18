#ifndef RAYTRACER_GRADIENT_H
#define RAYTRACER_GRADIENT_H

#include <utility>

#include "mfem.hpp"
#include <geometry.h>

namespace raytracer {
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
     * Gradient calculator that stores a grid function defined in H1 space and calculates its gradient.
     * */
    class H1Gradient : public Gradient {
    public:

        /**
         * Construct using a GridFunction defined in H1 on a given mesh

         * @param h1Function
         * @param mesh
         */
        explicit H1Gradient(mfem::GridFunction& h1Function, mfem::Mesh& mesh);

        /**
         * Return the value of gradient at pointOnFace
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
        mfem::GridFunction h1Function;
        mfem::Mesh& mesh;

        Vector getGradientAt(const Element &element, const Point &point) const;
    };

    /**
     * Take L2 GridFunction and project it on new H1 GridFunction
     * @param l2Function
     * @param l2Space
     * @param h1Space
     * @return
     */
    mfem::GridFunction projectL2toH1(
            mfem::GridFunction &l2Function,
            mfem::FiniteElementSpace &l2Space,
            mfem::FiniteElementSpace &h1Space
    );

    /**
     * GradientCalculator using gradient defined at nodal values to calculate gradient at face
     */
    class LinearInterGrad : public Gradient {
    public:
        explicit LinearInterGrad(std::map<Point *, Vector> gradientAtPoints) :
        gradientAtPoints(std::move(gradientAtPoints)) {}

        /**
         * Calculate the gradient in a point on a face by linear interpolation of gradient at nodes
         *
         * @param pointOnFace
         * @param previousElement
         * @param nextElement
         * @return
         */
        Vector get(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;

    private:
        std::map<Point *, Vector> gradientAtPoints;

        static Vector linearInterpolate(
                const Point &a,
                const Point &b,
                const Point &x,
                const Vector &valueA,
                const Vector &valueB);

    };

    /**
     * Calculate the gradient at nodes via LS solved by householder factorization
     * @param mesh
     * @param meshFunction to be used to calculate gradient
     * @return gradients at points
     */
    std::map<Point *, Vector> getHouseholderGradientAtPoints(const Mesh &mesh, const MeshFunction &meshFunction);
}


#endif //RAYTRACER_GRADIENT_H
