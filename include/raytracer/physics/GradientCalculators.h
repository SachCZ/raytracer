#ifndef RAYTRACER_GRADIENTCALCULATORS_H
#define RAYTRACER_GRADIENTCALCULATORS_H

#include "Vector.h"
#include "GeometryFunctions.h"
#include "mfem.hpp"
#include "Element.h"
#include "Point.h"

namespace raytracer {
    namespace physics {
        /**
         * Abstract interface to provide a GradientCalculator. To obey this interface implement the
         * getGradient method.
         */
        class GradientCalculator {
        public:
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
            explicit ConstantGradientCalculator(const geometry::Vector &gradient) : gradient(gradient) {}

            /**
             * Returns always the same Vector given at construction.
             * @return vector gradient.
             */
            geometry::Vector getGradient(const geometry::Intersection &) const override {
                return this->gradient;
            }

        private:
            const geometry::Vector gradient;
        };

        class H1GradientCalculator : public GradientCalculator {
        public:
            H1GradientCalculator(mfem::FiniteElementSpace &l2Space, mfem::FiniteElementSpace &h1Space):
                    l2Space(l2Space), h1Space(h1Space), _density(&h1Space) {}

            geometry::Vector getGradient(const geometry::Intersection &intersection) const override {
                auto point = intersection.orientation.point;
                auto previousGradient = this->getGradientAt(*intersection.previousElement, point);
                auto nextGradient = this->getGradientAt(*intersection.nextElement, point);
                return 0.5 * (previousGradient + nextGradient);
            }

            void updateDensity(mfem::GridFunction& density){
                this->_density = convertH1toL2(density);
            }

        private:
            mfem::FiniteElementSpace &l2Space;
            mfem::FiniteElementSpace &h1Space;
            mfem::GridFunction _density;

            geometry::Vector getGradientAt(const geometry::Element& element, const geometry::Point& point) const {
                mfem::Vector result(2);
                mfem::IntegrationPoint integrationPoint{};
                integrationPoint.Set2(point.x, point.y);

                auto transformation = this->h1Space.GetElementTransformation(element.getId());
                transformation->SetIntPoint(&integrationPoint);
                this->_density.GetGradient(*transformation, result);

                return {result[0], result[1]};
            }

            mfem::GridFunction convertH1toL2(const mfem::GridFunction &function) {
                mfem::BilinearForm A(&h1Space);
                A.AddDomainIntegrator(new mfem::MassIntegrator);
                A.Assemble();
                A.Finalize();

                mfem::MixedBilinearForm B(&l2Space, &h1Space);
                B.AddDomainIntegrator(new mfem::MassIntegrator);
                B.Assemble();
                B.Finalize();

                mfem::LinearForm b(&h1Space);
                B.Mult(function, b);

                mfem::DSmoother smoother(A.SpMat());

                mfem::GridFunction result(&h1Space);
                result = 0;
                mfem::PCG(A, smoother, b, result);
                return result;
            }
        };
    }
}


#endif //RAYTRACER_GRADIENTCALCULATORS_H
