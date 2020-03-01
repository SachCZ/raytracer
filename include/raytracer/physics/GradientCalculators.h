#ifndef RAYTRACER_GRADIENTCALCULATORS_H
#define RAYTRACER_GRADIENTCALCULATORS_H

#include "Vector.h"
#include "GeometryFunctions.h"
#include "mfem.hpp"
#include "Point.h"

namespace raytracer {
    namespace physics {
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
            virtual geometry::Vector getGradient(const geometry::Intersection &) const = 0;
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
            explicit ConstantGradientCalculator(const geometry::Vector &gradient);

            /**
             * Returns always the same Vector given at construction.
             * @return vector gradient.
             */
            geometry::Vector getGradient(const geometry::Intersection &) const override;

        private:
            const geometry::Vector gradient;
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
            geometry::Vector getGradient(const geometry::Intersection &intersection) const override;


            /**
             * Update the density from which the gradient is calculated. The density should be a function in L2 space.
             * @param density defined over L2
             */
            void updateDensity(mfem::GridFunction& density);

        private:
            mfem::FiniteElementSpace &l2Space;
            mfem::FiniteElementSpace &h1Space;
            mfem::GridFunction _density;

            geometry::Vector getGradientAt(const geometry::Element& element, const geometry::Point& point) const;

            mfem::GridFunction projectL2toH1(const mfem::GridFunction &function);
        };
    }
}


#endif //RAYTRACER_GRADIENTCALCULATORS_H
