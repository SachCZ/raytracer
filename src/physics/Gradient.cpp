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

    H1Gradient::H1Gradient(mfem::GridFunction &h1Function, mfem::Mesh &mesh) :
            h1Function(h1Function), mesh(mesh) {}

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

    Vector H1Gradient::getGradientAt(const Element &element, const Point &point) const {
        mfem::Vector result(2);
        mfem::IntegrationPoint integrationPoint{};
        integrationPoint.Set2(point.x, point.y);

        auto transformation = this->mesh.GetElementTransformation(element.getId());
        transformation->SetIntPoint(&integrationPoint);
        this->h1Function.GetGradient(*transformation, result);

        return {result[0], result[1]};
    }

    mfem::GridFunction projectL2toH1(
            mfem::GridFunction &l2Function,
            mfem::FiniteElementSpace &l2Space,
            mfem::FiniteElementSpace &h1Space
    ) {
        mfem::Array<int> attr(4);
        attr[0] = 1; //attr 1
        attr[1] = 0;
        attr[2] = 1;
        attr[3] = 0;

        mfem::GridFunctionCoefficient l2FunctionCoefficient(&l2Function);
        mfem::GridFunction h1Function(&h1Space);
        h1Function = 0;
        h1Function.ProjectCoefficient(l2FunctionCoefficient);

        mfem::MixedBilinearForm B(&l2Space, &h1Space);
        B.AddDomainIntegrator(new mfem::MixedScalarMassIntegrator);
        B.Assemble();
        B.Finalize();
        mfem::LinearForm b(&h1Space);
        B.Mult(l2Function, b);

        mfem::BilinearForm A(&h1Space);
        A.AddDomainIntegrator(new mfem::MassIntegrator);
        A.Assemble();
        A.EliminateEssentialBC(attr, h1Function, b);
        A.Finalize();

        mfem::GSSmoother smoother(A.SpMat());

        mfem::PCG(A, smoother, b, h1Function);

        return h1Function;
    }

    Vector NormalGradient::get(const PointOnFace &pointOnFace, const Element &previousElement,
                               const Element &nextElement) const {
        auto previousDensity = this->density.getValue(previousElement);
        auto nextDensity = this->density.getValue(nextElement);
        double factor = previousDensity > nextDensity ? -1 : 1;
        auto normal = pointOnFace.face->getNormal();
        return factor / normal.getNorm() * normal;
    }

    Vector LinearInterpolation::get(
            const PointOnFace &pointOnFace,
            const Element &,
            const Element &) const {
        auto points = pointOnFace.face->getPoints();
        auto it0 = this->gradientAtPoints.find(points[0]);
        auto it1 = this->gradientAtPoints.find(points[1]);
        if (it0 != this->gradientAtPoints.end() && it1 != this->gradientAtPoints.end()) {
            auto gradient0 = it0->second;
            auto gradient1 = it1->second;
            return linearInterpolate(*points[0], *points[1], pointOnFace.point, gradient0, gradient1);
        } else {
            throw std::logic_error("No gradient found!");
        }
    }

    Vector impl::getGradientAtPoint(const Mesh &mesh, const MeshFunction &meshFunction, const Point *point) {
        int index = 0;
        auto elements = mesh.getPointAdjacentElements(point);
        if (elements.size() < 3) {
            elements = {elements[0]};
            auto adjacent = mesh.getElementAdjacentElements(*elements[0]);
            elements.insert(elements.end(), adjacent.begin(), adjacent.end());
            if (elements.size() < 3) {
                elements = {elements[1]};
                adjacent = mesh.getElementAdjacentElements(*elements[1]);
                elements.insert(elements.end(), adjacent.begin(), adjacent.end());
            }
        }
        rosetta::Matrix A(elements.size(), 3);
        rosetta::Matrix b(elements.size(), 1);
        for (const auto &element : elements) {
            auto centroid = getElementCentroid(*element);
            auto dx = centroid.x - point->x;
            auto dy = centroid.y - point->y;
            auto d = dx * dx + dy * dy;
            auto weight = 1 / std::pow(d, 0.125);
            A(index, 0) = weight;
            A(index, 1) = weight * dx;
            A(index, 2) = weight * dy;
            b(index, 0) = weight * meshFunction.getValue(*element);
            ++index;
        }
        return impl::solveOverdetermined(A, b);
    }

    Vector LinearInterpolation::linearInterpolate(const Point &a, const Point &b, const Point &x, const Vector &valueA,
                                                  const Vector &valueB) {
        auto norm = (b - a).getNorm();
        auto xDistFromA = (x - a).getNorm();
        return valueA + (xDistFromA) / (norm) * (valueB - valueA);
    }

    Vector impl::solveOverdetermined(rosetta::Matrix &A, rosetta::Matrix &b) {
        rosetta::Matrix Q, R;
        householder(A, R, Q);
        Q.trim_columns(3);
        R.trim_rows(3);

        Q.transpose();
        rosetta::Matrix Qtb;
        Qtb.mult(Q, b);
        rosetta::Matrix x(3, 1);
        x.forward_substitute(R, Qtb);
        return {x(1, 0), x(2, 0)};
    }

    std::map<Point *, Vector> getHouseholderGradientAtPoints(const Mesh &mesh, const MeshFunction &meshFunction) {
        std::map<Point *, Vector> result;
        for (const auto &point : mesh.getPoints()) {
            result.insert({point, impl::getGradientAtPoint(mesh, meshFunction, point)});
        }
        return result;
    }
}

