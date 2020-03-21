#include "Gradient.h"

namespace raytracer {

    ConstantGradient::ConstantGradient(const Vector &gradient) : gradient(gradient) {}

    Vector ConstantGradient::get(
            const PointOnFace &,
            const Element &,
            const Element &
    ) const {
        return this->gradient;
    }

    H1Gradient::H1Gradient(
            mfem::FiniteElementSpace &l2Space,
            mfem::FiniteElementSpace &h1Space
    ) :
            l2Space(l2Space), h1Space(h1Space), _density(&h1Space) {}

    Vector H1Gradient::get(
            const PointOnFace &pointOnFace,
            const Element &previousElement,
            const Element &nextElement
    ) const {
        auto point = pointOnFace.point;
        auto previousGradient = this->getGradientAt(previousElement, point);
        auto nextGradient = this->getGradientAt(nextElement, point);
        return 0.5 * (previousGradient + nextGradient); //TODO look into this
    }

    void H1Gradient::updateDensity(mfem::GridFunction &density) {
        this->_density = this->projectL2toH1(density);
    }

    Vector
    H1Gradient::getGradientAt(const Element &element, const Point &point) const {
        mfem::Vector result(2);
        mfem::IntegrationPoint integrationPoint{};
        integrationPoint.Set2(point.x, point.y);

        auto transformation = this->h1Space.GetElementTransformation(element.getId());
        transformation->SetIntPoint(&integrationPoint);
        this->_density.GetGradient(*transformation, result);

        return {result[0], result[1]};
    }

    mfem::GridFunction H1Gradient::projectL2toH1(const mfem::GridFunction &function) {
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

