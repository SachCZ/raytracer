#ifndef RAYTRACER_GRADIENT_H
#define RAYTRACER_GRADIENT_H

#include <utility>

#include "mfem.hpp"
#include <geometry.h>

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

    /**
     * Gradient calculator that stores a grid function defined in H1 space and calculates its gradient.
     * */
    class H1Gradient {
    public:

        /**
         * Construct using a GridFunction defined in H1 on a given mesh

         * @param h1Function
         * @param mesh
         */
        explicit H1Gradient(mfem::GridFunction &h1Function, mfem::Mesh &mesh);

        /**
         * Return the value of gradient at pointOnFace
         *
         * @param pointOnFace
         * @param previousElement
         * @param nextElement
         * @return gradient at given point
         */
        Vector operator()(
                const PointOnFace &pointOnFace,
                const Element &previousElement,
                const Element &nextElement
        ) const;

    private:
        mfem::GridFunction h1Function;
        mfem::Mesh &mesh;

        Vector getGradientAt(const Element &element, const Point &point) const;
    };

    /**
     * Take L2 GridFunction and project it on new H1 GridFunction
     * @param rho
     * @param l2Space
     * @param h1Space
     * @return
     */
    VectorField mfemGradient(
            const MfemMesh &mesh,
            MfemMeshFunction &rho,
            mfem::VectorCoefficient *vectorBoundaryValue = nullptr,
            double diffusionC = 0,
            double meshH = 0
    );

    ScalarField mfemGradComp(
            mfem::GridFunction &rho,
            mfem::FiniteElementSpace &l2Space,
            const MfemMesh &mesh,
            mfem::Coefficient &boundaryValue,
            mfem::Coefficient &derivativeBoundaryValue
    );


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

    /**
     * Calculate the gradient at nodes via LS solved by householder factorization
     * @param mesh
     * @param meshFunction to be used to calculate gradient
     * @return gradients at points
     */
    VectorField calcHousGrad(const Mesh &mesh, const MeshFunc &meshFunction, bool includeBorder = true);

    VectorField setValue(const VectorField& grad, const std::vector<Point*>& points, const Vector& value);

    /**
     * Calculate the gradient in inner points of the mesh using integral over a curve.
     * This is a classic version assuming curve connecting centers of adjacent points.
     * It is only available for quadrilateral meshes
     * @param mesh
     * @param meshFunction which gradient is to be calculated
     * @return gradients at points
     */
    VectorField calcIntegralGrad(const Mesh &mesh, const MeshFunc &meshFunction);

    std::ostream &operator<<(std::ostream &os, const VectorField &VectorField);
}


#endif //RAYTRACER_GRADIENT_H
