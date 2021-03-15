#include <qr_decomposition.h>
#include "gradient.h"
#include <msgpack.hpp>

namespace raytracer {
    namespace impl {
        Vector solveOverdetermined(rosetta::Matrix &A, rosetta::Matrix &b) {
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


        Vector getGradientAtPoint(const Mesh &mesh, const MeshFunc &meshFunction, const Point *point) {
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
            auto result = solveOverdetermined(A, b);
            return result;
        }
    }

    ConstantGradient::ConstantGradient(const Vector &gradient) : gradient(gradient) {}

    Vector ConstantGradient::operator()(
            const PointOnFace &,
            const Element &,
            const Element &
    ) const {
        return this->gradient;
    }

    H1Gradient::H1Gradient(mfem::GridFunction &h1Function, mfem::Mesh &mesh) :
            h1Function(h1Function), mesh(mesh) {}

    Vector H1Gradient::operator()(
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

    ScalarField mfemGradComp(
            mfem::GridFunction &rho,
            mfem::FiniteElementSpace &l2Space,
            const MfemMesh &mesh,
            mfem::Coefficient &boundaryValue,
            mfem::Coefficient &derivativeBoundaryValue
    ) {
        mfem::Array<int> ess_bdr(4);
        ess_bdr = 0;
        ess_bdr[1] = 0;
        ess_bdr[3] = 1;

        mfem::H1_FECollection h1FiniteElementCollection{1, 2};
        mfem::FiniteElementSpace h1Space(mesh.getMfemMesh(), &h1FiniteElementCollection, 1);
        mfem::Array<int> ess_vdofs_list;
        h1Space.GetEssentialTrueDofs(ess_bdr, ess_vdofs_list);
        mfem::GridFunction solution(&h1Space);
        solution = 0;

        solution.ProjectBdrCoefficient(derivativeBoundaryValue, ess_bdr);

        mfem::ConstantCoefficient coeff(-1);
        mfem::MixedBilinearForm right_hand_side(&h1Space, &l2Space);
        right_hand_side.AddDomainIntegrator(new mfem::DerivativeIntegrator(coeff, 0));
        right_hand_side.Assemble();
        right_hand_side.Finalize();
        mfem::LinearForm b(&h1Space);
        mfem::ConstantCoefficient one(1);
        mfem::ConstantCoefficient minus_one(-1);
        mfem::Array<int> attr0(mesh.getMfemMesh()->bdr_attributes.Max()), attr1(mesh.getMfemMesh()->bdr_attributes.Max());
        attr0 = 0;
        attr0[1] = 1;
        attr0[3] = 0;
        attr1 = 0;
        attr1[1] = 0;
        attr1[3] = 1;

        mfem::Vector constCoeff(1);
        constCoeff[0] = 1;
        mfem::VectorConstantCoefficient coefficient(constCoeff);
        //b.AddBoundaryIntegrator(new mfem::BoundaryNormalLFIntegrator(coefficient));
        b.AddBoundaryIntegrator(new mfem::BoundaryLFIntegrator(boundaryValue), attr0);
        //b.AddBoundaryIntegrator(new mfem::BoundaryLFIntegrator(boundaryValue), attr1);
        b.Assemble();
        right_hand_side.AddMultTranspose(rho, b);

        mfem::BilinearForm a(&h1Space);
        a.AddDomainIntegrator(new mfem::MassIntegrator);
        a.Assemble();
        a.Finalize();

        mfem::SparseMatrix A;
        mfem::Vector B, X;
        a.FormLinearSystem(ess_vdofs_list, solution, b, A, X, B);

        mfem::DSmoother smoother(A);

        mfem::PCG(A, smoother, B, X);
        a.RecoverFEMSolution(X, b, solution);

        char vishost[] = "localhost";
        int visport = 19916;
        mfem::socketstream sol_sock(vishost, visport);
        sol_sock.precision(8);
        sol_sock << "solution\n" << *mesh.getMfemMesh() << solution << std::flush;
        return {};
    }

    mfem::Array<int> allBdrMarker(const mfem::Mesh& mesh){
        mfem::Array<int> marker(mesh.bdr_attributes.Max());
        marker = 1;
        return marker;
    }

    mfem::Array<int> getEssTrueDofs(const mfem::Array<int>& marker, mfem::FiniteElementSpace& space) {
        mfem::Array<int> essTrueDofs;
        space.GetEssentialTrueDofs(marker, essTrueDofs);
        return essTrueDofs;
    }

    VectorField gfToField(const MfemMesh& mesh, const mfem::GridFunction& function){
        auto dimSize = function.Size() / function.VectorDim();

        VectorField result;
        const auto &points = mesh.getInnerPoints();
        for (Point *point : points) {
            result[point] = Vector{function[point->id], function[dimSize + point->id]};
        }

        return result;
    }

    mfem::GridFunction solveByPCG(
            mfem::BilinearForm& leftSideMatrix,
            mfem::LinearForm& rightSideVector,
            const mfem::Array<int>& essTrueDofs,
            const mfem::GridFunction& initialValue
            ){
        mfem::SparseMatrix A;
        mfem::Vector B, X;
        mfem::GridFunction solution = initialValue;

        leftSideMatrix.FormLinearSystem(essTrueDofs, solution, rightSideVector, A, X, B);

        mfem::DSmoother smoother(A);
        mfem::PCG(A, smoother, B, X);
        leftSideMatrix.RecoverFEMSolution(X, rightSideVector, solution);
        return solution;
    }

    VectorField mfemGradient(
            const MfemMesh &mesh,
            MfemMeshFunction &rho,
            mfem::VectorCoefficient* vectorBoundaryValue,
            const double diffusionC,
            const double meshH
    ) {
        auto dim = mesh.getMfemMesh()->Dimension();
        mfem::H1_FECollection h1Fec{1, dim};
        mfem::FiniteElementSpace h1Space(mesh.getMfemMesh(), &h1Fec, dim);

        auto bdrMarker = allBdrMarker(*mesh.getMfemMesh());

        mfem::GridFunction initialValue(&h1Space);
        initialValue = 0;
        if (vectorBoundaryValue) {
            initialValue.ProjectBdrCoefficient(*vectorBoundaryValue, bdrMarker);
        }

        mfem::MixedBilinearForm rightSide(&h1Space, rho.getGF()->FESpace());
        mfem::ConstantCoefficient coefficient(-1);
        rightSide.AddDomainIntegrator(new mfem::VectorDivergenceIntegrator(coefficient));
        rightSide.Assemble();
        rightSide.Finalize();
        mfem::LinearForm rightSideVector(&h1Space);
        rightSide.MultTranspose(*rho.getGF(), rightSideVector);

        mfem::BilinearForm leftSideMatrix(&h1Space);
        leftSideMatrix.AddDomainIntegrator(new mfem::VectorMassIntegrator);
        mfem::ConstantCoefficient diffCoeff(diffusionC*meshH*meshH);
        if (diffusionC != 0) {
            leftSideMatrix.AddDomainIntegrator(new mfem::VectorDiffusionIntegrator(diffCoeff));
        }
        leftSideMatrix.Assemble();
        leftSideMatrix.Finalize();

        auto essTrueDofs = getEssTrueDofs(bdrMarker, h1Space);
        auto solution = solveByPCG(leftSideMatrix, rightSideVector, essTrueDofs, initialValue);

        char vishost[] = "localhost";
        int visport = 19916;
        mfem::socketstream sol_sock(vishost, visport);
        sol_sock.precision(8);
        sol_sock << "solution\n" << *mesh.getMfemMesh() << solution << std::flush;

        return gfToField(mesh, solution);
    }

    Vector LinInterGrad::operator()(
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

    Vector LinInterGrad::linearInterpolate(const Point &a, const Point &b, const Point &x, const Vector &valueA,
                                           const Vector &valueB) {
        auto norm2 = (b - a).getNorm2();
        auto xDistFromA2 = (x - a).getNorm2();
        auto factor = std::sqrt(xDistFromA2 / norm2);
        return valueA + factor * (valueB - valueA);
    }

    VectorField calcHousGrad(const Mesh &mesh, const MeshFunc &meshFunction) {
        VectorField result;
        for (const auto &point : mesh.getInnerPoints()) {
            result.insert({point, impl::getGradientAtPoint(mesh, meshFunction, point)});
        }
        return result;
    }

    bool isQuadMesh(const Mesh &mesh) {
        return mesh.getElements()[0]->getPoints().size() == 4;
    }

    double calcTriangleArea(const Point &a, const Point &b, const Point &c) {
        return std::abs(a.x * b.y + b.x * c.y + c.x * a.y - a.y * b.x - b.y * c.x - c.y * a.x) / 2.0;
    }

    VectorField calcIntegralGrad(const Mesh &mesh, const MeshFunc &meshFunction) {
        VectorField result;

        if (!isQuadMesh(mesh)) throw std::logic_error("Integral grad is only available for quads");

        for (Point *point : mesh.getInnerPoints()) {
            const auto &elements = mesh.getPointAdjOrderedElements(point);
            const auto &points = mesh.getPointAdjOrderedPoints(point);

            double gradX = 0;
            double gradY = 0;
            double volume = 0;
            for (size_t i = 0; i < elements.size(); i++) {
                size_t nextI = i + 1;
                if (i == elements.size() - 1) {
                    nextI = 0;
                }
                auto element = elements[i];
                auto value = meshFunction.getValue(*element);
                auto adjPoint = points[i];
                auto nextAdjPoint = points[nextI];
                gradX += (nextAdjPoint->y - adjPoint->y) * value;
                gradY -= (nextAdjPoint->x - adjPoint->x) * value;
                volume += calcTriangleArea(*point, *adjPoint, *nextAdjPoint);
            }
            gradX /= volume;
            gradY /= volume;
            result[point] = Vector{gradX, gradY};
        }
        return result;
    }

    std::ostream &operator<<(std::ostream &os, const VectorField &vectorField) {
        using namespace std;
        vector<pair<Point *, Vector>> pairs(begin(vectorField), end(vectorField));
        sort(begin(pairs), end(pairs), [](auto pairA, auto pairB) {
            return pairA.first->id < pairB.first->id;
        });
        vector<vector<double>> gradSerialization;
        gradSerialization.reserve(pairs.size());
        for (auto pair : pairs) {
            gradSerialization.emplace_back(vector<double>{pair.first->x, pair.first->y, pair.second.x, pair.second.y});
        }
        msgpack::pack(os, gradSerialization);
        return os;
    }
}

