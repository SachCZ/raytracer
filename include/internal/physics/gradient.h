#ifndef RAYTRACER_GRADIENT_H
#define RAYTRACER_GRADIENT_H

#include <utility>

#include "mfem.hpp"
#include <geometry.h>
#include <qr_decomposition.h>

namespace raytracer {
    /**
     * Vectors at points
     */
    using VectorField = std::map<Point *, Vector>;

    /**
     * Scalars at points
     */
    using ScalarField = std::map<Point *, double>;

    class Gradient {
    public:
        virtual tl::optional<Vector> get(const PointOnFace &pointOnFace) const = 0;
    };

    /**
     * GradientCalculator that returns a constant Vector no matter what.
     */
    class ConstantGradient : public Gradient {
    public:
        /**
         * Constructor that takes the Vector that will be returned every time as parameter.
         * @param gradient - the vector to be returned
         */
        explicit ConstantGradient(const Vector &gradient);

        /**
         * Returns always the same Vector given at construction.
         * @return vector gradient.
         */
        tl::optional<Vector> get(const PointOnFace &pointOnFace) const override;

    private:
        const Vector gradient;
    };

    namespace impl {
        mfem::Array<int> allBdrMarker(const mfem::Mesh &mesh);

        mfem::Array<int> getEssTrueDofs(const mfem::Array<int> &marker, mfem::FiniteElementSpace &space);

        VectorField gfToField(const MfemMesh &mesh, const mfem::GridFunction &function);

        mfem::GridFunction solveByPCG(
                mfem::BilinearForm &leftSideMatrix,
                mfem::LinearForm &rightSideVector,
                const mfem::Array<int> &essTrueDofs,
                const mfem::GridFunction &initialValue
        );
    }

    /**
     * Take L2 GridFunction and project it on new H1 GridFunction
     * @param rho
     * @param l2Space
     * @param h1Space
     * @return
     */
    template <typename MeshFunc>
    VectorField mfemGradient(
            const MfemMesh &mesh,
            MeshFunc &rho,
            mfem::VectorCoefficient *vectorBoundaryValue = nullptr,
            double diffusionC = 0,
            double meshH = 0
                    ){
        auto dim = mesh.getMfemMesh()->Dimension();
        mfem::H1_FECollection h1Fec{1, dim};
        mfem::FiniteElementSpace h1Space(mesh.getMfemMesh(), &h1Fec, dim);

        auto bdrMarker = impl::allBdrMarker(*mesh.getMfemMesh());

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

        auto essTrueDofs = impl::getEssTrueDofs(bdrMarker, h1Space);
        auto solution = impl::solveByPCG(leftSideMatrix, rightSideVector, essTrueDofs, initialValue);

        return impl::gfToField(mesh, solution);
    }


    /**
     * GradientCalculator using gradient defined at nodal values to calculate gradient at face
     */
    class LinInterGrad : public Gradient {
    public:
        /**
         * To construct this supply a gradient at points
         * @param gradientAtPoints
         */
        explicit LinInterGrad(VectorField gradientAtPoints) :
                gradientAtPoints(std::move(gradientAtPoints)) {}

        /**
         * Calculate the gradient in a point on a face by linear interpolation of gradient at nodes
         *
         * @param pointOnFace
         * @return
         */
        tl::optional<Vector> get(const PointOnFace &pointOnFace) const override;

        VectorField gradientAtPoints;
    private:

        static Vector linearInterpolate(
                const Point &a,
                const Point &b,
                const Point &x,
                const Vector &valueA,
                const Vector &valueB);

    };

    namespace impl {
        Vector solveOverdetermined(rosetta::Matrix &A, rosetta::Matrix &b);

        template <typename MeshFunc>
        Vector getGradientAtPoint(const Mesh &mesh, const MeshFunc &meshFunction, const Point *point) {
            int index = 0;
            auto elements = mesh.getPointAdjacentElements(point);
            if (elements.size() < 3) {
                elements = {elements[0]};
                auto adjacent = mesh.getElementAdjacentElements(*elements[0]);
                elements.insert(elements.end(), adjacent.begin(), adjacent.end());
                elements.emplace_back(nullptr);
            }

            rosetta::Matrix A(elements.size(), 3);
            rosetta::Matrix b(elements.size(), 1);
            for (const auto &element : elements) {
                Point centroid;
                double value;
                if (!element) {
                    auto elementCentroid = getElementCentroid(*elements[0]);
                    centroid = Point(Vector(*point) + (*point - elementCentroid));
                    value = 0;
                } else {
                    centroid = getElementCentroid(*element);
                    value = meshFunction[element->getId()];
                }

                auto dx = centroid.x - point->x;
                auto dy = centroid.y - point->y;
                auto d = dx * dx + dy * dy;
                auto weight = 1 / std::pow(d, 0.125);
                A(index, 0) = weight;
                A(index, 1) = weight * dx;
                A(index, 2) = weight * dy;
                b(index, 0) = weight * value;
                ++index;
            }
            auto result = solveOverdetermined(A, b);
            return result;
        }
    }

    /**
     * Calculate the gradient at nodes via LS solved by householder factorization
     * @param mesh
     * @param meshFunction to be used to calculate gradient
     * @return gradients at points
     */
    template<typename MeshFunc>
    VectorField calcHousGrad(const Mesh &mesh, const MeshFunc &meshFunction, bool includeBorder = true) {
        VectorField result;
        if (includeBorder) {
            for (const auto &point : mesh.getPoints()) {
                result.insert({point, impl::getGradientAtPoint(mesh, meshFunction, point)});
            }
        } else {
            for (const auto &point : mesh.getInnerPoints()) {
                result.insert({point, impl::getGradientAtPoint(mesh, meshFunction, point)});
            }
        }
        return result;
    }

    VectorField setValue(const VectorField &grad, const std::vector<Point *> &points, const Vector &value);

    namespace impl{
        bool isQuadMesh(const Mesh &mesh);
        double calcTriangleArea(const Point &a, const Point &b, const Point &c);
    }
    /**
     * Calculate the gradient in inner points of the mesh using integral over a curve.
     * This is a classic version assuming curve connecting centers of adjacent points.
     * It is only available for quadrilateral meshes
     * @param mesh
     * @param meshFunction which gradient is to be calculated
     * @return gradients at points
     */
    template <typename MeshFunc>
    VectorField calcIntegralGrad(const Mesh &mesh, const MeshFunc &meshFunction){
        VectorField result;

        if (!impl::isQuadMesh(mesh)) throw std::logic_error("Integral grad is only available for quads");

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
                auto value = meshFunction[element->getId()];
                auto adjPoint = points[i];
                auto nextAdjPoint = points[nextI];
                gradX += (nextAdjPoint->y - adjPoint->y) * value;
                gradY -= (nextAdjPoint->x - adjPoint->x) * value;
                volume += impl::calcTriangleArea(*point, *adjPoint, *nextAdjPoint);
            }
            gradX /= volume;
            gradY /= volume;
            result[point] = Vector{gradX, gradY};
        }
        return result;
    }

    std::ostream &operator<<(std::ostream &os, const VectorField &VectorField);
}


#endif //RAYTRACER_GRADIENT_H
