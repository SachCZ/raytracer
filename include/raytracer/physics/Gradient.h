#ifndef RAYTRACER_GRADIENT_H
#define RAYTRACER_GRADIENT_H

#include "Vector.h"
#include "Intersection.h"
#include "mfem.hpp"
#include "Point.h"

namespace raytracer {
    /**
     * Abstract interface to provide a GradientCalculator. To obey this interface implement the
     * getGradient method.
     */
    class GradientCalculator {
    public:
        /**
         * Override this.
         * @return the gradient at given intersection (point).
         */
        virtual Vector getGradient(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const = 0;
    };

    /**
     * GradientCalculator that returns a constant Vector no matter what.
     */
    class ConstantGradientCalculator : public GradientCalculator {
    public:
        /**
         * Constructor that takes the Vector that will be returned every time as parameter.
         * @param gradient - the vector to be returned
         */
        explicit ConstantGradientCalculator(const Vector &gradient);

        /**
         * Returns always the same Vector given at construction.
         * @return vector gradient.
         */
        Vector getGradient(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const override;

    private:
        const Vector gradient;
    };

    /**
     * GradientCalculator that stores a density grid function defined in H1 space obtained from function given
     * if L2. When getGradient is called the gradient of this function is calculated at given point.
     */
    class H1GradientCalculator : public GradientCalculator {
    public:
        /**
         * Constructor that expects the l2 and h1 spaces. L2 is the space in which density is usually defined
         * and H1 is the space into which the density is tranformed to be able to evaluate gradient.
         * @param l2Space
         * @param h1Space
         */
        H1GradientCalculator(
                mfem::FiniteElementSpace &l2Space,
                mfem::FiniteElementSpace &h1Space
        );

        /**
         * Return the value of gradient at the intersection point.
         * @param intersection
         * @return
         */
        Vector getGradient(
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

        mfem::GridFunction projectL2toH1(const mfem::GridFunction &function);
    };

    class StepGradient : public GradientCalculator {
    public:
        Vector getGradient(
                const PointOnFace &pointOnFace,
                const Element &,
                const Element &
        ) const override {
            //TODO fix direction
            return pointOnFace.face->getNormal();
        }
    };

}


#endif //RAYTRACER_GRADIENT_H