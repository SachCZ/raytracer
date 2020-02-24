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
            this->_density = convertH1toL2(density);
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

        mfem::GridFunction H1GradientCalculator::convertH1toL2(const mfem::GridFunction &function) {
            mfem::Array<int> ess_tdof_list;
            if (mesh.bdr_attributes.Size())
            {
                mfem::Array<int> ess_bdr(mesh.bdr_attributes.Max());
                ess_bdr = 1;
                l2Space.GetEssentialTrueDofs(ess_bdr, ess_tdof_list);
            }

            mfem::BilinearForm formA(&h1Space);
            formA.AddDomainIntegrator(new mfem::MassIntegrator);
            formA.Assemble();
            formA.Finalize();

            mfem::MixedBilinearForm formB(&l2Space, &h1Space);
            formB.AddDomainIntegrator(new mfem::MixedScalarMassIntegrator);
            formB.Assemble();
            formB.Finalize();

            mfem::LinearForm b(&h1Space);
            formB.Mult(function, b);

            mfem::GridFunction result(&h1Space);
            result = 0;

            mfem::OperatorPtr A;
            mfem::Vector B, X;
            formA.FormLinearSystem(ess_tdof_list, result, b, A, X, B);

            mfem::GSSmoother M((mfem::SparseMatrix&)(*A));
            PCG(*A, M, B, X, 0, 1000, 1e-12, 0.0);

            formA.RecoverFEMSolution(X, b, result);


            std::ofstream mesh_ofs("refined.mesh");
            mesh_ofs.precision(8);
            mesh.Print(mesh_ofs);
            std::ofstream sol_ofs("sol.gf");
            sol_ofs.precision(8);
            result.Save(sol_ofs);


            return result;
        }
    }
}

