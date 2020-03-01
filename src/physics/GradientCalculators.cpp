#include "GradientCalculators.h"

namespace raytracer {
    namespace physics {
        ConstantGradientCalculator::ConstantGradientCalculator(const geometry::Vector &gradient): gradient(gradient) {}

        geometry::Vector ConstantGradientCalculator::getGradient(const geometry::Intersection &) const {
            return this->gradient;
        }

        H1GradientCalculator::H1GradientCalculator(
                mfem::FiniteElementSpace &l2Space,
                mfem::FiniteElementSpace &h1Space,
                const mfem::Mesh& mesh):
                l2Space(l2Space), h1Space(h1Space), _density(&h1Space), mesh(mesh) {}

        geometry::Vector H1GradientCalculator::getGradient(const geometry::Intersection &intersection) const {
            auto point = intersection.orientation.point;
            if (intersection.previousElement && intersection.nextElement){
                auto previousGradient = this->getGradientAt(*intersection.previousElement, point);
                auto nextGradient = this->getGradientAt(*intersection.nextElement, point);
                return 0.5 * (previousGradient + nextGradient);
            } else if (intersection.nextElement){
                return this->getGradientAt(*intersection.nextElement, point);
            } else if (intersection.previousElement) {
                return this->getGradientAt(*intersection.previousElement, point);
            } else {
                throw std::logic_error("Intersection has no elements!");
            }
        }

        void H1GradientCalculator::updateDensity(mfem::GridFunction &density) {
            this->_density = this->projectL2toH1(density);
        }

        geometry::Vector
        H1GradientCalculator::getGradientAt(const geometry::Element &element, const geometry::Point &point) const {
            mfem::Vector result(2);
            mfem::IntegrationPoint integrationPoint{};
            integrationPoint.Set2(point.x, point.y);

            auto transformation = this->h1Space.GetElementTransformation(element.getId());
            transformation->SetIntPoint(&integrationPoint);
            this->_density.GetGradient(*transformation, result);

            return {result[0], result[1]};
        }

        mfem::GridFunction H1GradientCalculator::projectL2toH1(const mfem::GridFunction &function) {
            mfem::BilinearForm A(&h1Space);
            A.AddDomainIntegrator(new mfem::MassIntegrator);
            A.Assemble();
            A.Finalize();

            mfem::MixedBilinearForm B(&l2Space, &h1Space);
            B.AddDomainIntegrator(new mfem::MixedScalarMassIntegrator);
            B.Assemble();
            B.Finalize();

            mfem::LinearForm b(&h1Space);
            B.Mult(function, b);

            mfem::GSSmoother smoother(A.SpMat());

            mfem::GridFunction result(&h1Space);
            result = 0;
            mfem::PCG(A, smoother, b, result);

            return result;
        }
    }
}

