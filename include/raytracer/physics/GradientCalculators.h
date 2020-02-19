#ifndef RAYTRACER_GRADIENTCALCULATORS_H
#define RAYTRACER_GRADIENTCALCULATORS_H

#include "Vector.h"
#include "GeometryFunctions.h"
#include "mfem.hpp"
#include "Element.h"
#include "Point.h"

namespace raytracer {
    namespace physics {
        class GradientCalculator {
        public:
            virtual geometry::Vector getGradient(const geometry::Intersection &) const = 0;
        };

        class ConstantGradientCalculator : public GradientCalculator {
        public:
            explicit ConstantGradientCalculator(const geometry::Vector &gradient) : gradient(gradient) {}

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

                auto transformation = this->h1Space.GetElementTransformation(element.id);
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
