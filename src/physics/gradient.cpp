#include "gradient.h"
#include <msgpack.hpp>
#include <stdexcept>

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
    }

    ConstantGradient::ConstantGradient(const Vector &gradient) : gradient(gradient) {}

    tl::optional<Vector> ConstantGradient::get(const PointOnFace &) const {
        return this->gradient;
    }

    namespace impl {
        mfem::Array<int> allBdrMarker(const mfem::Mesh &mesh) {
            mfem::Array<int> marker(mesh.bdr_attributes.Max());
            marker = 1;
            return marker;
        }

        mfem::Array<int> getEssTrueDofs(const mfem::Array<int> &marker, mfem::FiniteElementSpace &space) {
            mfem::Array<int> essTrueDofs;
            space.GetEssentialTrueDofs(marker, essTrueDofs);
            return essTrueDofs;
        }

        VectorField gfToField(const MfemMesh &mesh, const mfem::GridFunction &function) {
            auto dimSize = function.Size() / function.VectorDim();

            VectorField result;
            const auto &points = mesh.getInnerPoints();
            for (Point *point : points) {
                result[point] = Vector{function[point->id], function[dimSize + point->id]};
            }

            return result;
        }

        mfem::GridFunction solveByPCG(
                mfem::BilinearForm &leftSideMatrix,
                mfem::LinearForm &rightSideVector,
                const mfem::Array<int> &essTrueDofs,
                const mfem::GridFunction &initialValue
        ) {
            mfem::SparseMatrix A;
            mfem::Vector B, X;
            mfem::GridFunction solution = initialValue;

            leftSideMatrix.FormLinearSystem(essTrueDofs, solution, rightSideVector, A, X, B);

            mfem::DSmoother smoother(A);
            mfem::PCG(A, smoother, B, X);
            leftSideMatrix.RecoverFEMSolution(X, rightSideVector, solution);
            return solution;
        }
    }

    tl::optional<Vector> LinInterGrad::get(const PointOnFace &pointOnFace) const {
        auto points = pointOnFace.face->getPoints();
        auto it0 = this->gradientAtPoints.find(points[0]);
        auto it1 = this->gradientAtPoints.find(points[1]);
        if (it0 != this->gradientAtPoints.end() && it1 != this->gradientAtPoints.end()) {
            auto gradient0 = it0->second;
            auto gradient1 = it1->second;
            return linearInterpolate(*points[0], *points[1], pointOnFace.point, gradient0, gradient1);
        } else {
            return {};
        }
    }

    Vector LinInterGrad::linearInterpolate(const Point &a, const Point &b, const Point &x, const Vector &valueA,
                                           const Vector &valueB) {
        auto norm2 = (b - a).getNorm2();
        auto xDistFromA2 = (x - a).getNorm2();
        auto factor = std::sqrt(xDistFromA2 / norm2);
        return valueA + factor * (valueB - valueA);
    }

    bool impl::isQuadMesh(const Mesh &mesh) {
        return mesh.getElements()[0]->getPoints().size() == 4;
    }

    double impl::calcTriangleArea(const Point &a, const Point &b, const Point &c) {
        return std::abs(a.x * b.y + b.x * c.y + c.x * a.y - a.y * b.x - b.y * c.x - c.y * a.x) / 2.0;
    }

    std::ostream &operator<<(std::ostream &os, const VectorField &vectorField) {
        using namespace std;
        vector<pair<Point *, Vector>> pairs(begin(vectorField), end(vectorField));
        sort(begin(pairs), end(pairs), [](pair<Point *, Vector> pairA, pair<Point *, Vector> pairB) {
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

    VectorField setValue(const VectorField &grad, const std::vector<Point *> &points, const Vector &value) {
        auto result = grad;
        for (Point *point : points) {
            if (grad.count(point)) {
                result.at(point) = value;
            } else {
                result[point] = value;
            }
        }
        return result;
    }
}

