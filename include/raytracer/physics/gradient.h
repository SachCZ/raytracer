#ifndef RAYTRACER_GRADIENT_H
#define RAYTRACER_GRADIENT_H

#include <utility>

#include "mfem.hpp"
#include <geometry.h>

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
        explicit H1Gradient(mfem::GridFunction& h1Function, mfem::Mesh& mesh);

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
        mfem::GridFunction h1Function;
        mfem::Mesh& mesh;

        Vector getGradientAt(const Element &element, const Point &point) const;
    };

    mfem::GridFunction projectL2toH1(
            mfem::GridFunction &l2Function,
            mfem::FiniteElementSpace &l2Space,
            mfem::FiniteElementSpace &h1Space
    );

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

    class LinearInterpolation : public Gradient {
    public:
        explicit LinearInterpolation(std::map<Point *, Vector> gradientAtPoints) : gradientAtPoints(
                std::move(gradientAtPoints)) {}

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

    std::map<Point *, Vector> getHouseholderGradientAtPoints(const Mesh &mesh, const MeshFunction &meshFunction);

    /**
     * @}
     */
}


#endif //RAYTRACER_GRADIENT_H
